// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/08/2016
// Description:

#ifndef COMMON_CODEC_UTILS_H
#define COMMON_CODEC_UTILS_H
#pragma once

#include <string>
#include <stdint.h>

namespace qcloud_cos {

class CommonCodecUtils {
public:

    /**
     * @brief 将字符x转成十六进制 (x的值[0, 15])
     *
     * @param x
     *
     * @return 十六进制字符
     */
    static unsigned char ToHex(const unsigned char& x);


    /**
     * @brief 对字符串进行URL编码
     *
     * @param str   带编码的字符串
     *
     * @return  经过URL编码的字符串
     */
    static std::string UrlEncode(const std::string& str);


    /**
     * @brief 对字符串进行base64编码
     *
     * @param plainText  待编码的字符串
     *
     * @return 编码后的字符串
     */
    static std::string Base64Encode(const std::string& plainText);

    /**
     * @brief 获取hmacSha1值
     *
     * @param plainText  明文
     * @param key        秘钥
     *
     * @return 获取的hmacsha1值
     */
    static std::string HmacSha1(const std::string& plainText,
                           const std::string &key);
    /**
     * @brief 获取文件的sha1值
     *
     * @param localFilePath  本地文件路径
     *
     * @return 文件的sha1值
     */
    static std::string GetFileSha1(const std::string& localFilePath);
};

} // namespace qcloud_cos
#endif // COMMON_CODEC_UTILS_H
