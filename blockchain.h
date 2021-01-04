#pragma once

#include <vector>
#include "block.h"

class blockchain {
public:
    std::vector<block> chain;

    void init(int difficulty) {
        chain.clear();
        
        block bl;

        bl.index = 0;
        bl.timestamp = time(NULL);
        bl.data = "Genesis";
        
        bl.mineBlockParalel(difficulty);

        chain.emplace_back(bl);
    }

    block getNextBlock() {
        block bl;
        block last = chain.back();

        bl.index = last.index + 1;
        bl.prev_hash = last.hash;
        bl.timestamp = time(NULL);

        return bl;
    }
};