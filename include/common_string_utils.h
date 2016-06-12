// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/10/2016
// Description:

#ifndef COMMON_STRING_UTILS_H
#define COMMON_STRING_UTILS_H
#pragma once

#include <stdint.h>
#include <string>
#include "json/json.h"

namespace qcloud_cos{

class CommonStringUtils {
public:

    /**
     * @brief 去除string两端的空格
     *
     * @param s: 待去除空格的字符串，是入参也是出参
     *
     * @return 返回去除空格后的字符串,即s
     */
    static std::string& Trim(std::string& s);

    /**
     * @brief 将json转为string
     *
     * @param jsonObject 需要转换json对象
     *
     * @return 返回转换后的string
     */
    static std::string JsonToString(const Json::Value& jsonObject);

    /**
     * @brief 将string转换为json
     *
     * @param jsonStr 待转换的string对象
     *
     * @return 返回转换后的json对象，转换失败返回NULL
     */
    static Json::Value StringToJson(const std::string& jsonStr);

    /**
     * @brief 把uint64_t类型的num转换成std::string,长度为8个字节
     *
     * @param num uint64_t类型
     *
     * @return 转换后的string
     */
    static std::string Uint64ToString(uint64_t num);

    /**
     * @brief 将int转为string
     *
     * @param num int类型
     *
     * @return 转换后的string
     */
    static std::string IntToString(int num);
};

} // namespace qcloud_cos
#endif // COMMON_STRING_UTILS_H
