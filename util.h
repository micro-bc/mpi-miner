#pragma once

#include <string>
#include <algorithm>

class Util {
public:
    static std::string to_hex(long o) {
        std::string ret;
        char t;

        for (int i = 0; i < sizeof(long)*2; ++i, o >>= 4) {
            t = o & 0xf;
            ret += t + (t < 10 ? '0' : 'a'-10);
        }
        std::reverse(ret.begin(), ret.end());

        return ret;
    }
};