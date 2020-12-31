#pragma once

#include "block.h"
#include <vector>
#include <string>

class blockchain {
public:
    std::vector<block> chain;

    blockchain() {
        block bl;

        bl.index = 0;
        bl.timestamp = time(NULL);
        bl.data = std::string("Genesis").c_str();
        
        bl.mineBlock(5);

        chain.emplace_back(bl);
    }
};