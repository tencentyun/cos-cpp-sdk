#ifndef _TENCENTYUN_COS_COSAPI_DEFINES__H_
#define _TENCENTYUN_COS_COSAPI_DEFINES__H_
#pragma once

#include <stdint.h>
#include <algorithm>
#include <string>
#include <map>

#include "common_string_utils.h"

namespace qcloud_cos {

const std::string kPathDelimiter = "/";

const unsigned char kPathDelimiterChar = '/';

// 默认的外部地址
const std::string kApiCosapiEndpoint = "http://web.file.myqcloud.com/files/v1/";

// 默认分片大小(Byte)
const int kDdefaultSliceSize = (3 * 1024 * 1024);

// 分片上传时，失败的最大重试次数
const int kMaxRetryTimes = 3;

struct CosApiClientOption {
    // APPID, 用户标识码，可在控制台获取
    uint64_t appid;
    // SECRET_ID/SECRET_KEY是用户的密钥对, 可在控制台获取
    std::string secret_id;
    std::string secret_key;
    // 超时时间(秒)
    uint64_t timeout_in_ms;
    // 全局超时时间
    uint64_t timeout_for_entire_request_in_ms;

    int64_t l5_modid;
    int64_t l5_cmdid;

    CosApiClientOption()
       : timeout_in_ms(30000),
         timeout_for_entire_request_in_ms(300 * 1000),
         l5_modid(-1), l5_cmdid(-1) {}  // default timeout 30s

    CosApiClientOption(uint64_t t_appid,
                       const std::string& t_secret_id,
                       const std::string& t_secret_key)
        : appid(t_appid), secret_id(t_secret_id),
          secret_key(t_secret_key), timeout_in_ms(30000),
          timeout_for_entire_request_in_ms(300 * 1000),
          l5_modid(-1), l5_cmdid(-1) {}

    CosApiClientOption(uint64_t t_appid,
                       const std::string& t_secret_id,
                       const std::string& t_secret_key,
                       uint64_t t_timeout_in_s)
        : appid(t_appid), secret_id(t_secret_id), secret_key(t_secret_key),
          timeout_in_ms(t_timeout_in_s), timeout_for_entire_request_in_ms(300 * 1000),
          l5_modid(-1), l5_cmdid(-1) {}

    CosApiClientOption(uint64_t t_appid,
                       const std::string& t_secret_id,
                       const std::string& t_secret_key,
                       uint64_t t_timeout_in_s,
                       uint64_t t_timeout_for_entire_request_in_ms)
        : appid(t_appid), secret_id(t_secret_id), secret_key(t_secret_key),
          timeout_in_ms(t_timeout_in_s),
          timeout_for_entire_request_in_ms(t_timeout_for_entire_request_in_ms),
          l5_modid(-1), l5_cmdid(-1) {}

    CosApiClientOption(const CosApiClientOption& option) {
        appid = option.appid;
        secret_id = option.secret_id;
        secret_key = option.secret_key;
        timeout_in_ms = option.timeout_in_ms;
        timeout_for_entire_request_in_ms = option.timeout_for_entire_request_in_ms;
        l5_modid = option.l5_modid;
        l5_cmdid = option.l5_cmdid;
    }
};

typedef std::map<std::string, std::string> StringOptionMap;
typedef StringOptionMap::const_iterator OptionConstIter;

class CustomOptions {
public:
    CustomOptions() {}

    void AddStringOption(const std::string& option_name,
                         const std::string& option_value) {
        m_options[option_name] = option_value;
    }

    void AddUIntOption(const std::string& option_name,
                      uint64_t option_value) {
        std::string option_value_str = CommonStringUtils::Uint64ToString(option_value);
        m_options[option_name] = option_value_str;
    }

    StringOptionMap GetAllOptions() const {
        return m_options;
    }

    void Clear() {
        m_options.clear();
    }

private:
    StringOptionMap m_options;
};

inline CustomOptions DefaultUploadOptions() {
    CustomOptions options;
    options.AddUIntOption("insertOnly", 1);
    return options;
}

inline CustomOptions DefaultUploadSliceOptions() {
    CustomOptions options;
    options.AddUIntOption("slice_size", kDdefaultSliceSize);
    options.AddStringOption("biz_attr", "");
    options.AddStringOption("op", "upload_slice");
    options.AddUIntOption("insertOnly", 1);
    return options;
}

inline CustomOptions DefaultListOptions() {
    CustomOptions options;
    options.AddUIntOption("num", 20);
    options.AddUIntOption("order", 0);
    options.AddStringOption("pattern", "eListBoth");
    options.AddStringOption("context", "");
    options.AddStringOption("op", "list");
    return options;
}

inline CustomOptions DefaultRenameFileOptions() {
    CustomOptions options;
    options.AddStringOption("op", "move");
    return options;
}

enum COSAPI_ERR_T {
    COSAPI_FILE_NOT_EXISTS = -1,
    COSAPI_NETWORK_ERROR = -2,
    COSAPI_PARAMS_ERROR = -3,
    COSAPI_ILLEGAL_SLICE_SIZE_ERROR = -4
};
} // namespace qcloud_cos

#endif // _TENCENTYUN_COS_COS_API_DEFINES_H_
