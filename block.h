#pragma once

#include <functional>
#include <string>
#include <thread>
#include "util.h"

class block
{
public:
    u_long index = 0;
    u_long hash;
    u_long prev_hash = 0;
    std::string data;
    u_long timestamp = 0;
    u_long nonce = 0;
    u_int diff = 1;

    std::string getDataToHash() {
        std::string data_to_hash;
        data_to_hash.resize(36);

        u_long *strPtr = (u_long *)data_to_hash.c_str();

        strPtr[0] = nonce;
        strPtr[1] = index;
        strPtr[2] = timestamp;
        strPtr[3] = prev_hash;
        *(int *)(&strPtr[4]) = diff;

        data_to_hash.append(data);

        return data_to_hash;
    }

    u_long getHash() {
        std::hash<std::string> hasher;
        return hasher(getDataToHash());
    }

    void mineBlockSolo(int difficulty, u_long startNonce = 0) {
        diff = difficulty;

        mineBlockSolo(getDataToHash(), difficulty, nonce, startNonce);

        hash = getHash();
    }

    void mineBlockParalel(int difficulty, u_long startNonce = 0, u_long proc_count = 0) {
        diff = difficulty;

        mineBlockParalel(getDataToHash(), difficulty, nonce, startNonce, proc_count);

        hash = getHash();
    }

    std::string to_string() {
        std::string ret;
        
        ret += "- Block --------";
        ret += "\n Index: " + std::to_string(index);
        ret += "\n Previous hash: " + Util::to_hex(prev_hash);
        ret += "\n Hash: " + Util::to_hex(hash);
        ret += "\n Timestamp: " + std::to_string(timestamp);
        ret += "\n Nonce: " + std::to_string(nonce);
        ret += "\n Difficulty: " + std::to_string(diff);
        ret += "\n----------------";

        return ret;
    }

    static void mineBlockSolo(std::string data_to_hash, int difficulty, u_long &result, u_long startNonce = 0) {
        result = 0;
        
        mineBlock(data_to_hash, difficulty, startNonce, result);
    }

    static void mineBlockParalel(std::string data_to_hash, int difficulty, u_long &result, u_long startNonce = 0, u_long proc_count = 0) {
        result = 0;

        int i;
        if (proc_count == 0) proc_count = std::thread::hardware_concurrency();

        std::thread *thr = new std::thread[proc_count];
        for (i = 0; i < proc_count; ++i)
            thr[i] = std::thread(mineBlock, data_to_hash, difficulty, startNonce + i, std::ref(result), proc_count);
        
        for (i = 0; i < proc_count; ++i)
            thr[i].join();
        
        delete[] thr;
    }
private:
    static void mineBlock(std::string data_to_hash, u_int diff, u_long startNonce, u_long &result, int inc = 1) {
        std::hash<std::string> hasher;
        long i, temp, mask = (1UL << diff*4) - 1;

        u_long &noncePtr = *(u_long *)data_to_hash.c_str(); // <3
        noncePtr = startNonce;

        while (!result) {
            temp = hasher(data_to_hash);
            
            if (!(temp & mask)) {
                result = noncePtr;
                return;
            }
            noncePtr += inc;
        }
    }
};