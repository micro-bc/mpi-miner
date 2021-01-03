#pragma once

#include <vector>
#include "block.h"

class blockchain {
public:
    std::vector<block> chain;

    blockchain() {
        block bl;

        bl.index = 0;
        bl.timestamp = time(NULL);
        bl.data = "Genesis";
        
        bl.mineBlockParalel(8);

        chain.emplace_back(bl);
    }
};