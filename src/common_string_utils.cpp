// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/10/2016
// Description:

#include "common_string_utils.h"

#include <cstdio>
#include <iostream>
#include <string>

using std::string;

namespace qcloud_cos {

string& CommonStringUtils::Trim(string &s) {
    if (s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

string CommonStringUtils::JsonToString(const Json::Value &jsonObject) {
    Json::FastWriter fast_writer;
    return fast_writer.write(jsonObject);
}

Json::Value CommonStringUtils::StringToJson(const string &jsonStr) {
    Json::Reader reader;
    Json::Value jsonObject;
    if (!reader.parse(jsonStr, jsonObject)) {
        std::cerr << "parse string to json error, origin str:" << jsonStr << std::endl;
    }
    return jsonObject;
}

string CommonStringUtils::Uint64ToString(uint64_t num) {
    char buf[65];
#if __WORDSIZE == 64
    snprintf(buf, sizeof(buf), "%lu", num);
#else
    snprintf(buf, sizeof(buf), "%llu", num);
#endif
    string str(buf);
    return str;
}

string CommonStringUtils::IntToString(int num) {
    char buf[65];
    snprintf(buf, sizeof(buf), "%d", num);
    string str(buf);
    return str;
}

} // namespace qcloud_cos
