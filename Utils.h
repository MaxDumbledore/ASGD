//
// Created by 40461 on 2021/4/29.
//

#ifndef ASGD_UTILS_H
#define ASGD_UTILS_H

#include <string>

std::string floatToBytes(const float &x);

float bytesToFloat(const std::string &s);

std::string intToBytes(const int &x);

int bytesToInt(const std::string &s);


#endif //ASGD_UTILS_H
