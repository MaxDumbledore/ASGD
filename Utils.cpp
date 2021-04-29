//
// Created by 40461 on 2021/4/29.
//

#include "Utils.h"

//
// Created by max on 2021/3/31.
//

#include "Utils.h"
#include <arpa/inet.h>
#include <cstring>

union Change {
    float g;
    uint32_t d;
    char data[4];
};

std::string floatToBytes(const float &x) {
    Change t;
    t.g = x;
    t.d = htonl(t.d);
    return t.data;
}

float bytesToFloat(const std::string &s) {
    Change t;
    std::memcpy(t.data, s.c_str(), 4);
    t.d = ntohl(t.d);
    return t.g;
}

std::string intToBytes(const int &x) {
    Change t;
    t.d = htonl(x);
    return t.data;
}

int bytesToInt(const std::string &s) {
    Change t;
    std::memcpy(t.data, s.c_str(), 4);
    return ntohl(t.d);
}
