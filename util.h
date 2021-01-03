#pragma once

#include <string>

class Util {
public:
    static std::string to_hex(long o) {
        std::string ret;
        ret.resize(sizeof(long)*2);

        for (int i = sizeof(long)*2 - 1; i >= 0; --i, o >>= 4) {
            ret[i] = o & 0xf;
            ret[i] += (ret[i] < 10 ? '0' : 'a'-10);
        }

        return ret;
    }

    static std::string to_hex(std::string o) {
        std::string ret;
        ret.resize(o.length() * 2);

        for (int i = o.length() * 2 - 1; i >= 0; --i) {
            ret[i] = o[i/2] & 0xf;
            ret[i] += (ret[i] < 10 ? '0' : 'a'-10);
            o[i/2] >>= 4;
        }

        return ret;
    }
};