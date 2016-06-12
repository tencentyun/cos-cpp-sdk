// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: rabbitliu <rabbitliu@tencent.com>
// Created: 05/27/16
// Description:

#include "../include/auth_utility.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../include/common_codec_utils.h"
#include "../include/cos_api_common.h"
#include "../include/true_random.h"

namespace qcloud_cos {

std::string AuthUtility::AppSignMuti(uint64_t app_id,
                                     const std::string& secret_id,
                                     const std::string& secret_key,
                                     uint64_t expired_time_in_s,
                                     const std::string& bucket_name) {
    // 多次可用签名的file_id为空
    return AppSignInternal(app_id, secret_id, secret_key, expired_time_in_s, "", bucket_name);
}

std::string AuthUtility::AppSignOnce(uint64_t app_id,
                                     const std::string& secret_id,
                                     const std::string& secret_key,
                                     const std::string& path,
                                     const std::string& bucket_name) {
    std::string urlencode_path = "/" + CosApiCommon::EncodePath(path);
    char file_id[2048];
    snprintf(file_id, sizeof(file_id),
#if __WORDSIZE == 64
             "/%lu/%s%s",
#else
             "/%llu/%s%s",
#endif
             app_id,
             bucket_name.c_str(),
             urlencode_path.c_str());

    return AppSignInternal(app_id, secret_id, secret_key, 0, file_id, bucket_name);
}

std::string AuthUtility::AppSignInternal(uint64_t app_id,
                                         const std::string& secret_id,
                                         const std::string& secret_key,
                                         uint64_t expired_time_in_s,
                                         const std::string& file_id,
                                         const std::string& bucket_name) {
    if (secret_id.empty() || secret_key.empty()) {
        // TODO(rabbitliu) 引入log
        return "";
    }

    time_t now = time(NULL);
    static TrueRandom true_random;
    uint64_t rdm = true_random.NextUInt64();
    char plain_text_char[10240];
    unsigned int input_length = snprintf(plain_text_char, 10240,
#if __WORDSIZE == 64
                                         "a=%lu&k=%s&e=%lu&t=%lu&r=%lu&f=%s&b=%s",
#else
                                         "a=%llu&k=%s&e=%llu&t=%lu&r=%llu&f=%s&b=%s",
#endif
                                         app_id, secret_id.c_str(), expired_time_in_s,
                                         now, rdm, file_id.c_str(), bucket_name.c_str());

    std::string plain_text(plain_text_char, input_length);
    std::string hmac_digest = CommonCodecUtils::HmacSha1(plain_text, secret_key);
    std::string encode_str = hmac_digest + plain_text;

    return CommonCodecUtils::Base64Encode(encode_str);
}

} // namespace qcloud_cos
