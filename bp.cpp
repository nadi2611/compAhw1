/* 046267 Computer Architecture - Spring 2020 - HW #1 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"

#include <exception>
#include <bitset>
#include <iostream>
#include <cmath>

using namespace std;

/*************************************************************************/
/*              CONSTANTS, TYPDEFS AND OTHER VEGETABLES                  */
/*************************************************************************/

#define OPCODE_BITS_LEN 32
#define MASK 0xFF
#define MAX_HISTORY_LENGTH 8

// For clarity, we used COMPARE function instead of performing comparison between two very long-named variables.
inline bool COMPARE(uint32_t a, uint32_t b) { return a == b; }

enum SHARE_MODE { NOT_SHARED = 0, LSB_SHARED = 1, MID_SHARED = 2};
enum STATE { SNT = 0, WNT = 1, WT = 2, ST = 3};

/*************************************************************************/
/*************************************************************************/
/*                          HISTORY HANDLER                              */
/*************************************************************************/
/*************************************************************************/

/*
 * Here we implement a class to support history table to store all the histories of each branch.
 * each entry in the array hold the current history of a specific branch. we access the entries
 * by the tag extracted from the branch pc. In case of GHR, only entry #0 is used, and all the
 * branches are mapped to it. The class supports the following methods:
 * getHistory - returns the history of the specific branch (if local) of the history stored in
 *              entry #0 if history is global (GHR).
 * Update - updates the history for specific branch (BHR) or for all the branches (GHR).
 * Reset - resets the history for specific branch (BHR) or for all the branches (GHR).
 */


class HistoryTable {

    /*********************************    HISTORY TABLE FIELDS    *********************************/

    bool is_history_global;
    unsigned history_size; // Consider removing

    // Mask usage explanation: History size may get the following values: 1...8 bits.
    // Since uint8_t is the smallest unsigned bit string we can use, we're bounded to
    // represent the history with 8 bits, and we need to make sure to always keep the
    // most significant bits which DO NOT belong to the history, zeroed. after history
    // update, we shift the history left, thus, sometimes, we turn on bits that aren't
    // part of the history. To zero them again, we perform history & mask, while using
    // a mask which has '1' in it's LSB's as the length of the history, and the rest of
    // the bits are '0'.
    uint8_t effective_history_bits = MASK;
    uint8_t *history_entry_array;


    /*********************************    HISTORY TABLE METHODS    *********************************/

public:
    HistoryTable(bool is_global_hist, const unsigned history_size, uint8_t btb_size) :
    is_history_global(is_global_hist), history_size(history_size) {

        history_entry_array = new uint8_t[btb_size];

        for(int i = 0; i < btb_size; i++) {
            resetHistory(i);
        }
        // Setup mask to extract effective bits from history
        effective_history_bits >>= (MAX_HISTORY_LENGTH - history_size);
    };

    ~HistoryTable() {
        if(history_entry_array != nullptr) delete[] history_entry_array;
    };

    // Return the history of a specific branch (in case of local BHR)
    // or return the history stored in entry #0 in case of using GHR.
    uint8_t getHistory(const uint8_t btb_index) const {
        if(is_history_global) {
            return history_entry_array[0];
        }

        return history_entry_array[btb_index];
    }

    void updateHistory(uint8_t btb_index, bool taken) {
        uint32_t key = is_history_global ? 0 : btb_index;
        history_entry_array[key] <<= 1;
        if(taken) {
            history_entry_array[key] += 1;
        }
        history_entry_array[key] = history_entry_array[key] & effective_history_bits;
        uint8_t hist = history_entry_array[key]; // TODO: REMOVE
        //printHistory(key);
    }

    void resetHistory(const uint8_t btb_index) {
        uint32_t key = is_history_global ? 0 : btb_index;
        history_entry_array[key] = 0;
    }

    // TODO: REMOVE. FOR DEBUG
    uint8_t printHistory(const uint8_t btb_index) {
        cout << history_entry_array[btb_index] << endl;
    }
};

/*************************************************************************/
/*************************************************************************/
/*                     PREDICTION TABLE HANDLER                          */
/*************************************************************************/
/*************************************************************************/


/*** #####################      PREDICTION ENTRY LOCAL FOR HISTORY      ##################### ***/

/*
 * Here we implement a class to support fsm machine for specific history. we access this class through the
 * prediction table, which stores all the FSM's for all the histories of a specific branches. each such machine
 * holds the prediction for this specific history. The class supports the following methods:
 * Predict - returns prediction for specific history.
 * Update - updates the fsm for specific history.
 * Reset - resets fsm for specific history.
 */

class PredictionFsmPerHistory {
    STATE current_state = WNT;

    /*********************************    FSM METHODS    *********************************/

public:
    PredictionFsmPerHistory() = default;
    ~PredictionFsmPerHistory() = default;

    bool predict() {
        if(current_state == SNT || current_state == WNT) {
            return false;
        } else {
            return true;
        }
    }

    void resetFsm(const STATE init_state) {
        current_state = init_state;
    }

    void updateFsm(bool taken) {
        // Make sure to not OF.
        if((current_state == ST && taken) || (current_state == SNT && !taken)) return;

        // For convenience, we saved the state as an enum. Since we cannot perform integer op's on this type,
        // we first cast it to int, perform the update, and then ast back again to STATE type which we defined.
        int state = static_cast<int>(current_state);
        state = taken ? (state + 1) : (state - 1);
        current_state = static_cast<STATE>(state);
    }

    // TODO: REMOVE. FOR DEBUG
    STATE getState() {
        return current_state;
    }
};


/*** #####################      PREDICTION TABLE LOCAL FOR BRANCH       ##################### ***/

/*
 * Here we implement a class to support per branch prediction table. we access this class through the prediction matrix,
 * which stores all the FSM's for all branches. each such machine holds 2^(history_size) entries, to store prediction
 * for every possible history. In case global_table flag is on, only one such table exists, and all the branches
 * are mapped to this one. The class supports the following methods:
 * Predict - returns prediction for specific branch (if local) & history.
 * Update - updates the fsm for specific branch (if local) & history.
 * Reset - resets fsm for specific branch (if local) & history.
 */

class PredictionTablePerBranch {
    unsigned table_size;
    PredictionFsmPerHistory *prediction_entries;
public:

    // Can't initialize an array in cpp using a C'tor so need special function to
    // loop other all entries and reset them at initialization.
    void _create_entries_array(unsigned history_size, STATE initial_state) {

        // Store table size for further use in resetFsm function.
        table_size = pow(2,history_size);

        // Allocate new table for all possible histories.
        prediction_entries = new PredictionFsmPerHistory[table_size];

        // Cannot initialize arrays in cpp.
        for(int i = 0; i < table_size; i++) {
            prediction_entries[i].resetFsm(initial_state);
        }
    }

    PredictionTablePerBranch() = default;

    ~PredictionTablePerBranch() {
        delete[] prediction_entries;
    };

    // Get prediction from the entry specified by history bits. In case of
    // global tables, we assume the table we're working on is shared between
    // all the branches. The PredictionMatrix which called us handled it.
    bool predict(uint8_t history_key) {
        return prediction_entries[history_key].predict();
    }

    // Update the state of the correct entry specified by history bits.
    //    - In case of global tables, we assume the table we're working on is
    //      shared between all the branches. The PredictionMatrix which called
    //      us handled it.
    //    - In case of GHR, we assume the right history key was given.
    void updateFsm(uint8_t history_key, bool taken) {
        prediction_entries[history_key].updateFsm(taken);
    }

    // At initialization and in case of a collision between two tags
    // at the same btb index, need to reset all the entries of this
    // specific branch to the initial state.
    void resetFsm(STATE initial_state) {
        for(int i = 0; i < table_size ; i++) {
            prediction_entries[i].resetFsm(initial_state);
        }
    }

    // TODO: REMOVE.FOR DEBUG
    void printArray() {
        for(int i=0; i< table_size; i++) {
            cout << "history key is: " << i << " state is: " << prediction_entries[i].getState() << endl;
        }
    }

};

/*** #############################      PREDICTION MATRIX       ############################# ***/


class PredictionMatrix {

    /*********************************    PREDICTION MATRIX FIELDS     *********************************/

    unsigned matrix_size;
    bool is_table_global;
    PredictionTablePerBranch *prediction_table;

    /*********************************    PREDICTION MATRIX METHODS     *********************************/

public:
    PredictionMatrix(unsigned btb_size, unsigned history_size, bool isTableGlobal, STATE initial_state) :
                    is_table_global(isTableGlobal) {
        this->matrix_size = btb_size;
        this->prediction_table = new PredictionTablePerBranch[btb_size];
        for(int i = 0; i < btb_size; i++){
            this->prediction_table[i]._create_entries_array(history_size, initial_state);
        }
    };

    ~PredictionMatrix() {
        delete[] this->prediction_table;
    }


    bool predict(uint8_t btb_index, uint8_t history_key) {
        if(is_table_global) {
            return this->prediction_table[0].predict(history_key);
        }
        else {
            return this->prediction_table[btb_index].predict(history_key);
        }
    }

    void updateFsm(uint8_t btb_index, uint8_t history_key, bool taken) {
        if(is_table_global) {
            prediction_table[0].updateFsm(history_key, taken);
            //prediction_table[0].printArray();// TODO: REMOVE. FOR DEBUG
        }
        else {
            prediction_table[btb_index].updateFsm(history_key, taken);
            //prediction_table[btb_index].printArray(); // TODO: REMOVE. FOR DEBUG
        }

    }

    void resetFsm(uint8_t btb_index, STATE initial_state) {
        if(is_table_global) {
            prediction_table[0].resetFsm(initial_state);
        }
        else {
            prediction_table[btb_index].resetFsm(initial_state);
        }
    }
};


/*************************************************************************/
/*************************************************************************/
/*                          PREDICTOR HANDLER                            */
/*************************************************************************/
/*************************************************************************/


/************    HELPER STRUCT TO STORE NECCESSARY INDICES    ************/

/*
 * Here we define a struct to store all the necessary indices extracted from the branch pc address.
 * the following indices are stored in this struct:
 *      btb index - used to access the correct btb entry to find the tag and target of this branch
 *      tag index - used to further identify the branch, and as a key to the history table and the
 *                  prediction state machine.
 *      shared index - in case of Lshare/Gshare this index stores the xored output of the history
 *                     bits with the adequate number of bits extracted from the given pc address.
 */

struct Indices {
    uint8_t btb_index = -1; // Needs maximum of 5 bits for representation. Closest adequate uintX_t type is uint8_t.
    uint32_t tag_index = -1; // Needs maximum 30 bits for representation. Closest adequate uintX_t type is uint32_t.
    uint8_t shared_index = -1; // Needs maximum 3 bits for representation. Closest adequate uintX_t type is uint8_t.
};

/*** #############################          BTB ENTRY           ############################# ***/

struct BTBEntry {
    uint32_t tag_identifier = -1;
    uint32_t target = 0;
public:
    BTBEntry() = default;
    ~BTBEntry() = default;
};

/*** ####################################################################################### ****/
/*** #############################          BTB TABLE           ############################# ***/
/*** ######################################################################################## ***/

/*
 * Here we implement a class to support the predictor. This class is our main object and controls all the operations.
 * Each branch first arrives to the btb table, if we see it for the first time, we cannot return a prediction , thus
 * we predict NT and return pc+4 as the destination. After the first acquaintance, the function BP_Update is called,
 * and the tag & target of this branch gain a new entry in the table. The btb holds also the history table and all the
 * prediction state machines of all branches for all possible histories, it then sends a command to the history table
 * object to reset the adequate entry, and sends a command to the prediction matrix to find the adequate prediction table
 * for this specific branch and to initialize all the entries. Similar thing happens if we encounter a known branch,
 * only in this case, the entry already exists, so we only need to update history and fsm, instead of resetting it.
 * In case of a collision between two branches with same btb index but different tags - we will reset the adequate
 * entries in history table and the prediction table. The class supports the following methods:
 * Predict - returns prediction for given branch pc.
 * Update - updates the history and fsm for given branch pc.
 * Reset - resets history and fsm for given branch pc.
 */

class BTBTable {

    /**************************************    BTB TABLE FIELDS    **************************************/

    unsigned btb_size;
    unsigned history_size;
    unsigned tag_size;
    STATE fsm_initial_state;

    bool is_history_global; // Consider removing
    bool is_fsm_global;
    SHARE_MODE share_mode;

    BTBEntry *btb_entries_array;
    HistoryTable history_table;
    PredictionMatrix prediction_matrix;
    SIM_stats statistics;

    /*********************************    BTB TABLE HELPER FUNCTIONS    *********************************/

    unsigned _calc_total_size() const {
        unsigned size = 0;
        if(is_history_global && is_fsm_global){
            size = btb_size * (tag_size + 1 + (OPCODE_BITS_LEN - 2)) + history_size + (2 * pow(2,history_size));
        } else if(!is_history_global && is_fsm_global) {
            size = btb_size * (tag_size + 1 + (OPCODE_BITS_LEN - 2) + history_size) + (2 * pow(2,history_size));
        } else if (!is_history_global && is_fsm_global) {
            size = btb_size * (tag_size + 1 + (OPCODE_BITS_LEN - 2) + (2 * pow(2,history_size))) + history_size;
        } else {
            size = btb_size * (tag_size + 1 + (OPCODE_BITS_LEN - 2) + history_size + (2 * pow(2,history_size)));
        }
        return size;
    }

    bool _btb_entry_exists(const uint8_t btb_index) const {
        return (btb_entries_array[btb_index].tag_identifier != -1);
    }

    void _insert_btb_entry(Indices &indices, uint32_t targetPc) {
        btb_entries_array[indices.btb_index].tag_identifier = indices.tag_index;
        btb_entries_array[indices.btb_index].target = targetPc;

        if(!is_history_global) history_table.resetHistory(indices.btb_index);
        if(!is_fsm_global) prediction_matrix.resetFsm(indices.btb_index, fsm_initial_state);
    }

    void _update_entrance_info(const Indices &indices, const uint32_t target) {
        // Update tag and target in adequate entrance
        btb_entries_array[indices.btb_index].tag_identifier = indices.tag_index;
        btb_entries_array[indices.btb_index].target = target;
    }

    void _reset_btb_entry_upon_tag_collision(const Indices &indices, const uint32_t new_target) {
        // Update BTBEntry with new tag and target
        _update_entrance_info(indices, new_target);
        history_table.resetHistory(indices.btb_index);
        prediction_matrix.resetFsm(indices.btb_index, fsm_initial_state);
    }

    bool _verify_entry(const Indices &indices) {
        return (!COMPARE(btb_entries_array[indices.btb_index].tag_identifier,-1) &&
                (COMPARE(btb_entries_array[indices.btb_index].tag_identifier, indices.tag_index)));
    }

    uint32_t _get_bits_by_indices(const uint32_t address, const unsigned start, const unsigned end) {
        if(start == end) return 0;
        uint32_t decoded_bits = address;
        decoded_bits <<= (OPCODE_BITS_LEN - end);
        decoded_bits >>=  (start + OPCODE_BITS_LEN - end);
        return decoded_bits;
    }

    void _decode_indices(const uint32_t branch_pc, Indices &indices) {

        // Extract btb entry index. Opcodes are aligned to 4 bytes.
        // Ignore the two LS bits since they are always equal to zero.
        uint32_t no_alignment_bits_addr = _get_bits_by_indices(branch_pc, 2, OPCODE_BITS_LEN);
        indices.btb_index = _get_bits_by_indices(no_alignment_bits_addr, 0, log2(btb_size));
        indices.tag_index = _get_bits_by_indices(no_alignment_bits_addr,  log2(btb_size),  log2(btb_size) + tag_size);

        uint8_t pc_bits_to_share;
        uint8_t history_bits = history_table.getHistory(indices.btb_index);
        switch (share_mode) {
            case NOT_SHARED:
                indices.shared_index = -1;
                break;
            case LSB_SHARED:
                pc_bits_to_share = _get_bits_by_indices(no_alignment_bits_addr, 0, history_size);
                indices.shared_index = pc_bits_to_share ^ history_bits;
                break;
            case MID_SHARED:
                pc_bits_to_share = _get_bits_by_indices(no_alignment_bits_addr, 14, 14 + history_size);
                indices.shared_index = pc_bits_to_share ^ history_bits;
                break;
            default:
                indices.shared_index = -1;
        }
    }

    /*********************************    BTB TABLE PUBLIC METHODS    *********************************/

public:
    BTBTable(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState, bool isGlobalHist,
              bool isGlobalTable, int Shared) :
              btb_size(btbSize), history_size(historySize), tag_size(tagSize),
              fsm_initial_state(static_cast<STATE>(fsmState)), is_history_global(isGlobalHist),
              is_fsm_global(isGlobalTable), share_mode(static_cast<SHARE_MODE>(Shared)),
              btb_entries_array(new BTBEntry[btbSize]), history_table(isGlobalHist, historySize, btbSize),
              prediction_matrix(btbSize, historySize, isGlobalTable, static_cast<STATE>(fsmState)),
              statistics{0,0,_calc_total_size()} {};

    ~BTBTable() {
        // No need to destroy entries first since we store the entry itself and not a pointer
        // to the entry, thus, the D'tor will be called automatically
        if(btb_entries_array != nullptr) delete[] btb_entries_array;
    }

    bool predict(const uint32_t branch_pc, uint32_t *target) {

        bool prediction = false;

        // Extract all necessary indices.
        auto *indices = new Indices();
        _decode_indices(branch_pc, *indices);

        // Check if entrance already exist and whether it has the right tag
        if(_verify_entry(*indices)) {

            // The currently handled branch already has an entry allocated for it and has the right tag.
            // No need to reset, just get index to the fsm and return the prediction and target.
            *target = btb_entries_array[indices->btb_index].target;
            uint8_t hist = history_table.getHistory(indices->btb_index); // TODO: REMOVE. For Debug
            uint8_t history_key = (share_mode != NOT_SHARED) ? indices->shared_index : history_table.getHistory(indices->btb_index);
            prediction = prediction_matrix.predict(indices->btb_index,history_key);

            // If the currently handled branch has the same btb index but DIFFERENT tag.
            // No need to reset entry or anything, just return false.
        }

        delete indices;

        // Entry does not exist or doesn't have the right tag
        return prediction;
    }

    void updateBtb(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst) {
        statistics.br_num++;

        auto *indices = new Indices();
        _decode_indices(pc, *indices);

        // TODO: REMOVE. FOR DEBUG
        //cout << "shared fsm key is: " << (unsigned)indices->shared_index << endl;

        // Update statistics
        if((!taken && (pred_dst != pc+4)) || (taken && (pred_dst == pc+4)) || (taken && (pred_dst != targetPc))) {
            statistics.flush_num++;
        }

        // Check if entry exists
        if(_btb_entry_exists(indices->btb_index)){

            // Entry exists. Check for collision between tags. Reset if needed.
            if(!COMPARE(btb_entries_array[indices->btb_index].tag_identifier, indices->tag_index)) {
                //_reset_btb_entry_upon_tag_collision(*indices, targetPc);

                uint8_t history_key = (share_mode != NOT_SHARED) ? indices->shared_index : history_table.getHistory(indices->btb_index);

                if(!is_fsm_global) prediction_matrix.resetFsm(indices->btb_index, fsm_initial_state);
                if(!is_history_global) {
                    history_table.resetHistory(indices->btb_index);
                    Indices updated_indices1;
                    _decode_indices(pc,updated_indices1);
                    history_key = (share_mode != NOT_SHARED) ? updated_indices1.shared_index : history_table.getHistory(updated_indices1.btb_index);;
                }

                //uint8_t history_key = (share_mode != NOT_SHARED) ? indices->shared_index : history_table.getHistory(indices->btb_index);

                _update_entrance_info(*indices, targetPc);

                //uint8_t history_key = (share_mode != NOT_SHARED) ? indices->shared_index : history_table.getHistory(indices->btb_index);
                prediction_matrix.updateFsm(indices->btb_index, history_key, taken);
                history_table.updateHistory(indices->btb_index, taken);

                delete indices;

                //cout << endl << endl; // TODO: REMOVE. FOR DEBUG.


                return;
            }

            // Update BTB
            this->btb_entries_array[indices->btb_index].target = targetPc;
            uint8_t history_key = (share_mode != NOT_SHARED) ? indices->shared_index : history_table.getHistory(indices->btb_index);
            history_table.updateHistory(indices->btb_index, taken);
            prediction_matrix.updateFsm(indices->btb_index, history_key, taken);
        } else {
            _insert_btb_entry(*indices, targetPc);
            uint8_t history_key = (share_mode != NOT_SHARED) ? indices->shared_index : history_table.getHistory(indices->btb_index);
            history_table.updateHistory(indices->btb_index, taken);
            prediction_matrix.updateFsm(indices->btb_index, history_key, taken);
        }

        delete indices;

        //cout << endl << endl; // TODO: REMOVE. FOR DEBUG.
    }

    void getStat(SIM_stats &update_stat) const {
        update_stat.br_num = statistics.br_num;
        update_stat.flush_num = statistics.flush_num;
        update_stat.size = statistics.size;
    }
};


BTBTable* branch_predictor = nullptr;

int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared){
    if ((btbSize != 1 && btbSize != 2 && btbSize != 4 && btbSize != 8 && btbSize != 16 && btbSize != 32) ||
    (historySize < 1 || historySize > 8) || (tagSize < 1 || tagSize > 30 - log2(btbSize)) ||
    (fsmState < 0  || fsmState > 3))
        return -1;
    try{
        branch_predictor = new BTBTable(btbSize, historySize, tagSize, fsmState, isGlobalHist,
                           isGlobalTable, Shared);
    } catch (bad_alloc&) {
        return -1;
    }
    return 0;
}

bool BP_predict(uint32_t pc, uint32_t *dst){
    if (branch_predictor->predict(pc, dst)){
        return true;
    } else {
        *dst = pc + 4;
        return false;
    }
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
    branch_predictor->updateBtb(pc, targetPc, taken, pred_dst);
	return;
}

void BP_GetStats(SIM_stats *curStats){

    branch_predictor->getStat(*curStats);

    // By TA's documentation this function will be called once at the end of a test set
    // and may be used to free allocated data

    // I have NO idea why, but this "delete" causes windows to be VERY angry,
    // and throws 0xC0000374 exception at me, so I just commented this line.
    // Need to make sure it still runs OK on their machine...
    //if(branch_predictor != nullptr) delete branch_predictor;
	return;
}

