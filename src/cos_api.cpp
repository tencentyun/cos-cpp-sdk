#include "cos_api.h"

#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "auth_utility.h"
#include "common_codec_utils.h"
#include "common_file_utils.h"
#include "common_string_utils.h"
#include "cos_api_common.h"
#include "cos_api_defines.h"
#include "httpsender.h"
#ifdef _USE_L5
#include "l5_endpoint_provider.h"
#endif

using std::string;
using std::map;
namespace qcloud_cos {

bool g_init = false;

// 预防多线程反复Init

class SimpleMutex {
public:
    SimpleMutex() {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~SimpleMutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void Lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void Unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

// mutex holder
class SimpleMutexLocker {
public:
    SimpleMutexLocker(SimpleMutex* mutex) : m_mutex(mutex) {
        m_mutex->Lock();
    }

    ~SimpleMutexLocker() {
        m_mutex->Unlock();
    }

private:
    SimpleMutex* m_mutex;
};

// 防止反复Init
SimpleMutex init_mutex;

int COS_Init() {
    SimpleMutexLocker locker(&init_mutex);
    if (g_init) {
        return 0;
    }

    g_init = true;
    CURLcode retCode = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != retCode){
        std::cerr << "curl_global_init error:" << CURLE_OK << std::endl;
        return 1;
    }
    return 0;
}

void COS_UInit() {
    SimpleMutexLocker locker(&init_mutex);
    if (g_init) {
        curl_global_cleanup();
        g_init = false;
    }
}

CosApi::CosApi(const CosApiClientOption& option) {
    m_client_option = option;
}

CosApi::~CosApi() {
}

string CosApi::Upload(
        const string &srcPath,
        const string &bucketName,
        const string &dstPath,
        const CustomOptions& options) {
    int access_ret = access(srcPath.c_str(), F_OK | R_OK);
    if (access_ret != 0) {
        Json::Value retJson;
        retJson["code"] = COSAPI_FILE_NOT_EXISTS;
        retJson["message"] = "file not exist or can not be read...";
        return CommonStringUtils::JsonToString(retJson);
    }

    if (!CosApiCommon::IsLegalFilePath(dstPath)) {
        Json::Value retJson;
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = dstPath + " is invalid file path!";
        return CommonStringUtils::JsonToString(retJson);
    }

    string fullUrl = GetEncodedCosUrl(bucketName, dstPath, m_client_option);

    uint64_t expired = CosApiCommon::GetExpiredTime();
    string sign = AuthUtility::AppSignMuti(m_client_option.appid,
                                m_client_option.secret_id,
                                m_client_option.secret_key,
                                expired,
                                bucketName);

    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    string sha1Digest = CommonCodecUtils::GetFileSha1(srcPath);
    //std::cout << "file: " << srcPath << ", sha1: " << sha1Digest << std::endl;
    std::map<string, string> http_params;
    http_params["op"] = "Upload";
    http_params["sha"] = sha1Digest;

    FillHttpParams(options, &http_params);

    string fileContent = CommonFileUtils::GetFileContent(srcPath);
    return HttpSender::SendSingleFilePostRequest(fullUrl,
                                                 http_headers,
                                                 http_params,
                                                 (unsigned char *)fileContent.c_str(),
                                                 fileContent.length(),
                                                 m_client_option);
}

string CosApi::UploadSliceInternal(
        const string &srcPath,
        const string &bucketName,
        const string &dstPath,
        const CustomOptions& options,
        bool parallel) {
    int access_ret = access(srcPath.c_str(), F_OK | R_OK);
    if (access_ret != 0) {
        Json::Value retJson;
        retJson["code"] = COSAPI_FILE_NOT_EXISTS;
        retJson["message"] = "file not exist or can not be read...";
        return CommonStringUtils::JsonToString(retJson);
    }

    if (!CosApiCommon::IsLegalFilePath(dstPath)) {
        Json::Value retJson;
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = dstPath + " is invalid file path!";
        return CommonStringUtils::JsonToString(retJson);
    }

    string fullUrl = GetEncodedCosUrl(bucketName, dstPath, m_client_option);
    uint64_t expired = CosApiCommon::GetExpiredTime();
    string sign = AuthUtility::AppSignMuti(m_client_option.appid,
                                m_client_option.secret_id,
                                m_client_option.secret_key,
                                expired,
                                bucketName);

    string shaDigest = CommonCodecUtils::GetFileSha1(srcPath);
    uint64_t fileSize = CommonFileUtils::GetFileLen(srcPath);

    std::map<string, string> http_params;
    CustomOptions new_options = options;
    new_options.AddStringOption("op", "upload_slice");
    new_options.AddStringOption("sha", shaDigest);
    new_options.AddUIntOption("filesize", fileSize);
    FillHttpParams(new_options, &http_params);

    string cmd_ret = UploadSliceCmd(fullUrl, sign, http_params);
    std::cout << "cmd_ret: " << cmd_ret << std::endl;
    Json::Value retJson = CommonStringUtils::StringToJson(cmd_ret);
    // 分片上传第一步返回错误,则终止
    if (retJson["code"].asInt() != 0) {
        return cmd_ret;
    }

    // 表示秒传，则结束
    if (retJson["data"].isMember("access_url")) {
        return cmd_ret;
    }

    if (parallel) {
        std::map<std::string, std::string> http_headers;
        http_headers["Authorization"] = sign;

        http_params.clear();
        http_params["op"] = "upload_slice";
        http_params["session"] = retJson["data"]["session"].asString();
        return HttpSender::SendFileParall(fullUrl,
                                          http_headers,
                                          http_params,
                                          srcPath,
                                          retJson["data"]["offset"].asUInt64(),
                                          retJson["data"]["slice_size"].asUInt64(),
                                          m_client_option);
    } else {
        uint64_t sliceSizeRet = retJson["data"]["slice_size"].asUInt64();
        uint64_t offset = retJson["data"]["offset"].asUInt64();
        string sessionId = retJson["data"]["session"].asString();
        unsigned char* sliceBuf = new unsigned char[sliceSizeRet + 1];
        memset(sliceBuf, 0, (sliceSizeRet + 1));

        std::ifstream fInput(srcPath.c_str(), std::ios::in | std::ios::binary);
        fInput.seekg(offset, std::ios::beg);
        uint64_t slice_len = 0;

        string slice_ret = "";
        while(offset < fileSize) {
            fInput.read((char*)sliceBuf, sliceSizeRet);
            slice_len = fInput.gcount();
            slice_ret = UploadSliceData(fullUrl,
                                          sign,
                                          sessionId,
                                          offset,
                                          sliceBuf,
                                          slice_len);
            retJson = CommonStringUtils::StringToJson(slice_ret);
            if (retJson["code"].asInt() != 0) {
                break;
            }
            offset += slice_len;
        }

        delete[] sliceBuf;
        fInput.close();
        return slice_ret;
    }
}

string CosApi::UploadSlice(
        const string &srcPath,
        const string &bucketName,
        const string &dstPath,
        const CustomOptions& options) {
    return UploadSliceInternal(srcPath, bucketName, dstPath, options, false);
}

string CosApi::UploadSliceCmd(
        const string &url,
        const string sign,
        const std::map<std::string, std::string>& http_params) {
    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    return HttpSender::SendSingleFilePostRequest(
        url, http_headers, http_params, NULL, 0, m_client_option);
}

string CosApi::UploadSliceData(
        const string &url,
        const string &sign,
        const string &session,
        const uint64_t offset,
        const unsigned char *sliceContent,
        const uint64_t sliceLen) {
    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    std::map<string, string> http_params;
    http_params["op"] = "upload_slice";
    http_params["session"] = session;
    http_params["offset"] = CommonStringUtils::Uint64ToString(offset);

    int retry_times = 0;
    string sliceSendRetStr = "";
    do {
        sliceSendRetStr = HttpSender::SendSingleFilePostRequest(url,
            http_headers, http_params, sliceContent, sliceLen, m_client_option);
        Json::Value jsonRet = CommonStringUtils::StringToJson(sliceSendRetStr);

        if (jsonRet["code"].asInt() == 0) {
            return sliceSendRetStr;
        }
        ++retry_times;
    } while (retry_times < kMaxRetryTimes);

    return sliceSendRetStr;
}

string CosApi::CreateFolder(
        const string &bucketName,
        const string &path,
        const CustomOptions& options) {

    if (!CosApiCommon::isLegalFolderPath(path)) {
        Json::Value retJson;
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = path + " is invalid folder path!";
        return CommonStringUtils::JsonToString(retJson);
    }
    string fullUrl = GetEncodedCosUrl(bucketName, path, m_client_option);
    // std::cout << "fullUrl: " << fullUrl << std::endl;

    uint64_t expired = CosApiCommon::GetExpiredTime();
    string sign = AuthUtility::AppSignMuti(m_client_option.appid,
                                m_client_option.secret_id,
                                m_client_option.secret_key,
                                expired,
                                bucketName);


    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    std::map<string, string> http_params;
    http_params["op"] = "create";
    FillHttpParams(options, &http_params);
    return HttpSender::SendJsonPostRequest(
        fullUrl, http_headers, http_params, m_client_option);
}

string CosApi::ListFolder(
        const string &bucketName,
        const string &path,
        const CustomOptions& options) {
    string folderPath = CosApiCommon::FormatFolderPath(path);
    return ListBase(bucketName, folderPath, options);
}

string CosApi::PrefixSearch(
    const string &bucketName,
    const string &prefix,
    const CustomOptions& options) {
    string filePath = CosApiCommon::FormatFilePath(prefix);
    return ListBase(bucketName, filePath, options);
}

string CosApi::ListBase(
        const string &bucketName,
        const string &path,
        const CustomOptions& options) {
    string fullUrl = GetEncodedCosUrl(bucketName, path, m_client_option);
    uint64_t expired = CosApiCommon::GetExpiredTime();
    string sign = AuthUtility::AppSignMuti(m_client_option.appid,
                                m_client_option.secret_id,
                                m_client_option.secret_key,
                                expired,
                                bucketName);

    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    std::map<string, string> http_params;
    FillHttpParams(options, &http_params);
    return HttpSender::SendGetRequest(
        fullUrl, http_headers, http_params, m_client_option);
}

string CosApi::UpdateFolder(
        const string &bucketName,
        const string &path,
        const CustomOptions& options) {
    if (!CosApiCommon::isLegalFolderPath(path)) {
        Json::Value retJson;
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = path + " is invalid folder path!";
        return CommonStringUtils::JsonToString(retJson);
    }
    return UpdateBase(bucketName, path, options);
}

string CosApi::UpdateFile(
    const string &bucketName,
    const string &path,
    const CustomOptions& options) {

    if (!CosApiCommon::IsLegalFilePath(path)) {
        Json::Value retJson;
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = path + " is invalid file path!";
        return CommonStringUtils::JsonToString(retJson);
    }
    return UpdateBase(bucketName, path, options);
}

string CosApi::UpdateBase(
    const string &bucketName,
    const string &path,
    const CustomOptions& options) {

    if (CosApiCommon::IsRootPath(path)) {
        Json::Value retJson;
        string retMsg = "can not update bucket use api! go to http://console.qcloud.com/cos to operate bucket";
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = retMsg;
        return CommonStringUtils::JsonToString(retJson);
    }

    string formatPathStr = CosApiCommon::FormatPath(path);
    string fullUrl = GetEncodedCosUrl(bucketName, formatPathStr, m_client_option);

    string sign = AuthUtility::AppSignOnce(m_client_option.appid,
                                m_client_option.secret_id,
                                m_client_option.secret_key,
                                formatPathStr,
                                bucketName);

    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    std::map<string, string> http_params;
    http_params["op"] = "update";
    FillHttpParams(options, &http_params);

    return HttpSender::SendJsonPostRequest(
        fullUrl, http_headers, http_params, m_client_option);
}

string CosApi::StatFolder(
    const string &bucketName,
    const string &path) {
    string folderPath = CosApiCommon::FormatFolderPath(path);
    return StatBase(bucketName, folderPath);
}

string CosApi::StatFile(
    const string &bucketName,
    const string &path) {

    string filePath = CosApiCommon::FormatFilePath(path);
    return StatBase(bucketName, filePath);
}

string CosApi::StatBase(
    const string &bucketName,
    const string &path) {

    string fullUrl = GetEncodedCosUrl(bucketName, path, m_client_option);

    std::map<string, string> http_params;
    http_params["op"] = "stat";

    uint64_t expired = CosApiCommon::GetExpiredTime();
    string sign = AuthUtility::AppSignMuti(m_client_option.appid,
                                m_client_option.secret_id,
                                m_client_option.secret_key,
                                expired,
                                bucketName);
    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    return HttpSender::SendGetRequest(
        fullUrl, http_headers, http_params, m_client_option);
}

string CosApi::DelFolder(
    const string &bucketName,
    const string &path) {

    if (!CosApiCommon::isLegalFolderPath(path)) {
        Json::Value retJson;
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = path + " is invalid folder path!";
        return CommonStringUtils::JsonToString(retJson);
    }
    return DelBase(bucketName, path);
}

string CosApi::DelFile(
    const string &bucketName,
    const string &path) {

    if (!CosApiCommon::IsLegalFilePath(path)) {
        Json::Value retJson;
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = path + " is invalid file path!";
        return CommonStringUtils::JsonToString(retJson);
    }
    return DelBase(bucketName, path);
}

string CosApi::DelBase(
    const string &bucketName,
    const string &path) {
    string formatPathStr = CosApiCommon::FormatPath(path);
    string sign = AuthUtility::AppSignOnce(m_client_option.appid,
                                     m_client_option.secret_id,
                                     m_client_option.secret_key,
                                     formatPathStr,
                                     bucketName);

    string fullUrl = GetEncodedCosUrl(bucketName, formatPathStr, m_client_option);

    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    std::map<string, string> http_params;
    http_params["op"] = "delete";

    return HttpSender::SendJsonPostRequest(
        fullUrl, http_headers, http_params, m_client_option);
}

std::string CosApi::MoveFile(
        const std::string& bucketName,
        const std::string& srcPath,
        const std::string& dstPath,
        const CustomOptions& options) {
    if (CosApiCommon::IsRootPath(srcPath) || CosApiCommon::IsRootPath(dstPath)) {
        Json::Value retJson;
        string retMsg = "can not delete bucket use api! go to http://console.qcloud.com/cos to operate bucket";
        retJson["code"] = COSAPI_PARAMS_ERROR;
        retJson["message"] = retMsg;
        return CommonStringUtils::JsonToString(retJson);
    }

    string formatPathStr = CosApiCommon::FormatPath(srcPath);
    string sign = AuthUtility::AppSignOnce(m_client_option.appid,
                                           m_client_option.secret_id,
                                           m_client_option.secret_key,
                                           formatPathStr,
                                           bucketName);

    string fullUrl = GetEncodedCosUrl(bucketName, formatPathStr, m_client_option);

    std::map<string, string> http_headers;
    http_headers["Authorization"] = sign;

    std::map<string, string> http_params;
    http_params["dest_fileid"] = dstPath;
    http_params["op"] = "move";
    FillHttpParams(options, &http_params);
    return HttpSender::SendJsonPostRequest(
        fullUrl, http_headers, http_params, m_client_option);
}

void CosApi::FillHttpParams(const CustomOptions& options,
                            std::map<string, string>* http_params) {
    std::map<std::string, std::string> option_map = options.GetAllOptions();
    for (OptionConstIter it = option_map.begin(); it != option_map.end(); ++it) {
        http_params->insert(std::make_pair(it->first, it->second));
    }
}
std::string CosApi::GetEncodedCosUrl(const std::string& bucket_name,
                                     const std::string& path,
                                     const CosApiClientOption& options) {
#ifdef __USE_L5
    if (options.l5_modid == -1 && options.l5_cmdid == -1) {
        return CosApiCommon::GetEncodedCosUrl(kApiCosapiEndpoint, bucket_name, path, options.appid);
    }
    // l5地址
    std::string endpoint;
    if (!L5EndpointProvider::GetEndPoint(m_client_option.l5_modid,
                                         m_client_option.l5_cmdid, &endpoint)) {
        return CosApiCommon::GetEncodedCosUrl(kApiCosapiEndpoint,
                                              bucket_name, path,
                                              options.appid);
    }
    return CosApiCommon::GetEncodedCosUrl(endpoint, bucket_name, path, options.appid);
#else
    return CosApiCommon::GetEncodedCosUrl(kApiCosapiEndpoint, bucket_name, path, options.appid);
#endif
}

}//namespace qcloud_cos
