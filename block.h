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
        int i, j = 0;

        long temp = nonce;
        for (i = 0; i < sizeof(long); ++i, ++j, temp >>= 8)
            data_to_hash[j] = (char)temp;

        temp = index;
        for (i = 0; i < sizeof(long); ++i, ++j, temp >>= 8)
            data_to_hash[j] = (char)temp;

        temp = timestamp;
        for (i = 0; i < sizeof(long); ++i, ++j, temp >>= 8)
            data_to_hash[j] = (char)temp;

        temp = diff;
        for (i = 0; i < sizeof(int); ++i, ++j, temp >>= 8)
            data_to_hash[j] = (char)temp;

        temp = prev_hash;
        for (i = 0; i < sizeof(long); ++i, ++j, temp >>= 8)
            data_to_hash[j] = (char)temp;

        data_to_hash.append(data);

        return data_to_hash;
    }

    u_long getHash() {
        std::hash<std::string> hasher;
        return hasher(getDataToHash());
    }

    void mineBlockSolo(int difficulty, u_long startNonce = 0) {
        diff = difficulty;
        nonce = 0;

        std::string data_to_hash = getDataToHash();
        
        mineBlock(data_to_hash, diff, startNonce, nonce);

        hash = getHash();
    }

    void mineBlockParalel(int difficulty, u_long startNonce = 0) {
        diff = difficulty;
        nonce = 0;
        
        int i;
        u_long proc_count = std::thread::hardware_concurrency();

        std::string data_to_hash = getDataToHash();

        std::thread *thr = new std::thread[proc_count];
        for (i = 0; i < proc_count; ++i)
            thr[i] = std::thread(mineBlock, data_to_hash, diff, startNonce + i, std::ref(nonce), proc_count);
        
        for (i = 0; i < proc_count; ++i)
            thr[i].join();
        
        delete[] thr;

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

private:
    static void mineBlock(std::string data_to_hash, u_int diff, u_long startNonce, u_long &result, int inc = 1) {
        std::hash<std::string> hasher;
        long i, temp, mask = (1UL << diff*4) - 1;

        while (!result) {
            temp = startNonce;
            for (i = 0; i < sizeof(long); ++i, temp >>= 8)
                data_to_hash[i] = (char)temp;
            
            temp = hasher(data_to_hash);
            
            if ((temp & mask) == 0) {
                result = startNonce;
                return;
            }
            startNonce += inc;
        }
    }
};