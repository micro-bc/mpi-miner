#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <string.h>
#include <thread>
#include "util.h"

class block
{
public:
    long index = 0;
    long hash;
    long prev_hash = 0;
    const char* data;
    long timestamp = 0;
    long nonce = 0;
    int diff = 1;

    u_long getHash() {
        std::string data_to_hash;
        int i;

        long temp = index;
        for (i = 0; i < sizeof(long); ++i, temp >>= 8)
            data_to_hash += (char)temp;

        temp = timestamp;
        for (i = 0; i < sizeof(long); ++i, temp >>= 8)
            data_to_hash += (char)temp;

        temp = nonce;
        for (i = 0; i < sizeof(long); ++i, temp >>= 8)
            data_to_hash += (char)temp;

        temp = diff;
        for (i = 0; i < sizeof(int); ++i, temp >>= 8)
            data_to_hash += (char)temp;

        temp = prev_hash;
        for (i = 0; i < sizeof(long); ++i, temp >>= 8)
            data_to_hash += (char)temp;

        data_to_hash.append(data);

        std::hash<std::string> hasher;
        return hasher(data_to_hash);
    }

    block clone() {
        block ret;

        ret.index = index;
        ret.hash = hash;
        ret.prev_hash = prev_hash;
        ret.timestamp = timestamp;
        ret.nonce = nonce;
        ret.diff = diff;

        int n = sizeof(data);
        ret.data = new char[n];
        // strncpy(ret.data, data, n);

        return ret;
    }

    void mineBlock(int difficulty, long startNonce = 0) {
        nonce = startNonce;
        diff = difficulty;

        while (true) {
            hash = getHash();
            if (((hash >> (diff*4)) << (diff*4)) == hash)
                return;
            ++nonce;
        }
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
};