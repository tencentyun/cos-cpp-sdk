// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/10/2016
// Description:

#ifndef COMMON_FILE_UTILS_H
#define COMMON_FILE_UTILS_H
#pragma once

#include <string>
#include <stdint.h>

namespace qcloud_cos {

class CommonFileUtils {
public:

    /**
     * @brief 获取文件内容
     *
     * @param localFilePath     本地文件地址
     *
     * @return                  返回文件内容
     */
    static std::string GetFileContent(const std::string& localFilePath);


    /**
     * @brief                   获取文件长度
     *
     * @param localFilePath     本地文件地址
     *
     * @return                  返回文件长度
     */
    static uint64_t GetFileLen(const std::string& localFilePath);
};

} // namespace qcloud_cos

#endif // COMMON_FILE_UTILS_H
