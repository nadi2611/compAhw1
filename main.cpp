//
// Created by elino on 11/13/2021.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>

#include "bp_api.h"

using namespace std;

void testExample1() {
    uint32_t src, target, dst = 0;
    bool taken;
    const char* prediction;

    BP_init(2,2, 26, 1, false, false, 0);

    src = 0x1230;
    target = 0x12300;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x1230;
    target = 0x12300;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x1230;
    target = 0x12300;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x10c;
    target = 0x200;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);


    SIM_stats simStats;
    BP_GetStats(&simStats);

    cout << "flush_num: " << simStats.flush_num << ", br_num: " << simStats.br_num << ", size: " << dec << simStats.size << "b" << endl;
}

void testExample2() {
    uint32_t src, target, dst = 0;
    bool taken;
    const char* prediction;

    BP_init(2,2, 26, 1, true, true, 0);

    src = 0x1230;
    target = 0x12300;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x1230;
    target = 0x12300;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x1230;
    target = 0x12300;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x10c;
    target = 0x200;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);


    SIM_stats simStats;
    BP_GetStats(&simStats);

    cout << "flush_num: " << simStats.flush_num << ", br_num: " << simStats.br_num << ", size: " << dec << simStats.size << "b" << endl;
}

void testExample3() {
    uint32_t src, target, dst = 0;
    bool taken;
    const char* prediction;

    BP_init(2,8, 26, 1, true, true, 1);

    src = 0x10;
    target = 0x20;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x10;
    target = 0x20;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x10;
    target = 0x20;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x10;
    target = 0x20;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x10;
    target = 0x20;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x14;
    target = 0x24;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x14;
    target = 0x24;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x14;
    target = 0x24;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x14;
    target = 0x24;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x14;
    target = 0x24;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);


    SIM_stats simStats;
    BP_GetStats(&simStats);

    cout << dec << "flush_num: " << simStats.flush_num << ", br_num: " << dec << simStats.br_num << ", size: " << dec << simStats.size << "b" << endl;
}

void testExample4() {
    uint32_t src, target, dst = 0;
    bool taken;
    const char* prediction;

    BP_init(32,3, 20, 2, false, true, 1);

    src = 0x17C;
    target = 0x17C;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17C;
    target = 0x17C;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    SIM_stats simStats;
    BP_GetStats(&simStats);

    cout << dec << "flush_num: " << simStats.flush_num << ", br_num: " << dec << simStats.br_num << ", size: " << dec << simStats.size << "b" << endl;
}

void testExample5() {
    uint32_t src, target, dst = 0;
    bool taken;
    const char* prediction;

    BP_init(1,2, 26, 2, true, true, 0);

    src = 0x1230;
    target = 0x12300;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x1230;
    target = 0x12300;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x1230;
    target = 0x12300;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x10c;
    target = 0x200;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x87654;
    target = 0x45678;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    SIM_stats simStats;
    BP_GetStats(&simStats);

    cout << dec << "flush_num: " << simStats.flush_num << ", br_num: " << dec << simStats.br_num << ", size: " << dec << simStats.size << "b" << endl;
}

void testExample6() {
    uint32_t src, target, dst = 0;
    bool taken;
    const char* prediction;

    BP_init(32,3, 20, 2, false, true, 1);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = false;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x27c;
    target = 0x27c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    src = 0x17c;
    target = 0x17c;
    taken = true;
    prediction = BP_predict(src, &dst) ? "T" : "N";
    cout << "0x" << hex << src << " " << prediction << " " << "0x" << hex << dst << endl;
    BP_update(src, target, taken, dst);

    SIM_stats simStats;
    BP_GetStats(&simStats);

    cout << dec << "flush_num: " << simStats.flush_num << ", br_num: " << dec << simStats.br_num << ", size: " << dec << simStats.size << "b" << endl;
}

int main() {
    //testExample1();
    //testExample2();
    //testExample3();
    //testExample4();
    //testExample5();
    testExample6();
    return 0;
}