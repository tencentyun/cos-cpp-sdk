#include "cos_api_common.h"

#include <stdio.h>
#include "common_codec_utils.h"
#include "cos_api_defines.h"

using std::string;

namespace qcloud_cos{

string CosApiCommon::FormatFolderPath(const string &path) {
    string folderPathStr = path;
    size_t len = folderPathStr.length();
    if (len > 0 && folderPathStr[len - 1] != kPathDelimiterChar) {
        folderPathStr.append(kPathDelimiter);
    }
    return FormatPath(folderPathStr);
}

string CosApiCommon::FormatFilePath(const string &path) {
    string filePathStr = path;
    size_t len = filePathStr.length();
    if (len > 0 && filePathStr[len - 1] == kPathDelimiterChar) {
        filePathStr.erase(len - 1, 1);
    }
    return FormatPath(filePathStr);
}

bool CosApiCommon::IsLegalFilePath(const string &path) {
    size_t len = path.length();
    if (len == 0 || path[0] != kPathDelimiterChar || path[len -1] == kPathDelimiterChar) {
        return false;
    } else {
        return true;
    }
}

bool CosApiCommon::isLegalFolderPath(const string &path) {
    size_t len = path.length();
    if (len == 0 || path[0] != kPathDelimiterChar || path[len -1] != kPathDelimiterChar) {
        return false;
    } else {
        return true;
    }
}

bool CosApiCommon::IsRootPath(const string &dstPath) {
    return (dstPath == kPathDelimiter);
}

string CosApiCommon::EncodePath(const string &dstPath) {
    string encodeStr = "";
    if (dstPath.empty()) {
        return encodeStr;
    }
    if (dstPath.find(kPathDelimiter) != 0) {
        encodeStr.append(kPathDelimiter);
    }
    size_t total_len = dstPath.length();
    size_t pos = 0;
    size_t next_pos = 0;
    string tmp_str;
    while (pos < total_len) {
        next_pos = dstPath.find(kPathDelimiter, pos);
        if (next_pos == string::npos) {
            next_pos = total_len;
        }
        tmp_str = dstPath.substr(pos, next_pos - pos);
        CommonStringUtils::Trim(tmp_str);
        if (!tmp_str.empty()) {
            encodeStr.append(CommonCodecUtils::UrlEncode(tmp_str));
            encodeStr.append(kPathDelimiter);
        }
        pos = next_pos + 1;
    }

    // 如果原路径是文件，则删除最后一个分隔符
    if (dstPath[total_len - 1] != kPathDelimiterChar) {
        encodeStr.erase(encodeStr.length() - 1, 1);
    }
    return encodeStr;
}

string CosApiCommon::GetEncodedCosUrl(const string& endpoint,
                                      const string& bucketName,
                                      const string& dstPath,
                                      int appid) {
    char urlBytes[10240];
    snprintf(urlBytes, sizeof(urlBytes),
#if __WORDSIZE == 64
             "/%llu/%s/%s",
#else
             "/%lu/%s/%s",
#endif
             appid,
             bucketName.c_str(),
             dstPath.c_str());

    string url_str(urlBytes);
    return endpoint + EncodePath(url_str);
}

uint64_t CosApiCommon::GetExpiredTime() {
    return time(NULL) + 60;
}


string CosApiCommon::FormatPath(const string &path) {
    if(path.empty()) {
        return path;
    }
    string formatStr = kPathDelimiter;
    size_t len = path.length();
    size_t last_delimiter_pos = -1;
    for(size_t i = 0; i < len; ++i) {
        if (path[i] == kPathDelimiterChar && last_delimiter_pos + 1 == i) {
            last_delimiter_pos = i;
            continue;
        } else if (path[i] == kPathDelimiterChar && last_delimiter_pos + 1 < i) {
            last_delimiter_pos = i;
            formatStr.append(1, path[i]);
        } else {
            formatStr.append(1, path[i]);
        }
    }
    return formatStr;
}
} // namespace qcloud_cos{
