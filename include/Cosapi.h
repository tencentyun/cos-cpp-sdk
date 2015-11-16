#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include "json/json.h"
#include "curl/curl.h"

#include "Auth.h"

#ifndef _TENCENTYUN_COS_COSAPI_H_H_
#define _TENCENTYUN_COS_COSAPI_H_H_

using namespace std;

namespace Qcloud_cos{

class Cosapi
{

public:
    static const string API_COSAPI_END_POINT;

    static const int EXPIRED_SECONDS;
    static const int DEFAULT_SLICE_SIZE;
    static const int MIN_SLICE_FILE_SIZE;
    static const int MAX_RETRY_TIMES;
    
    const uint64_t APPID;
    const string SECRET_ID;
    const string SECRET_KEY;

    enum COSAPI_ERR_T {
        COSAPI_FILE_NOT_EXISTS = -1,
        COSAPI_NETWORK_ERROR = -2,
        COSAPI_PARAMS_ERROR = -3,
        COSAPI_ILLEGAL_SLICE_SIZE_ERROR = -4
    };

    static int32_t global_init();
    static void global_finit();

    void reset();

    Cosapi(
        const uint64_t appid, 
        const string &secretId,
        const string &secretKey,
        const uint64_t timeout = 30);
    virtual ~Cosapi();


    int sendRequest(
            const string &url,
            const int isPost,
            const vector<string> *headers,
            const char *data = NULL,
            struct curl_httppost * form_data = NULL);

    string generateResUrl(
            const string &bucketName,
            const string &dstPath);

    /**
     * 上传文件
     * @param  string  srcPath     本地文件路径
     * @param  string  bucketName  上传的bcuket名称
     * @param  string  dstPath     上传的文件路径
     * @return [type]                [description]
     */
    int upload(
            const string &srcPath,
            const string &bucketName,
            const string &dstPath,
            const string &bizAttr = "");

    /**
     * 上传文件
     * @param  string  srcPath     本地文件路径
     * @param  string  bucketName  上传的bcuket名称
     * @param  string  dstPath     上传的文件路径
     * @return [type]                [description]
     */
    int upload_slice(
            const string &srcPath,
            const string &bucketName,
            const string &dstPath,
            const string &bizAttr = "",
            const int sliceSize = 0,
            const string &session = "");


    /*
     * 创建目录
     * @param  string  bucketName
     * @param  string  path 目录路径，sdk会补齐末尾的 '/'
     *
     */
    int createFolder(
        const string &bucketName, 
        const string &path,
        const string &biz_attr = ""
            ); 

    /*
     * 目录列表
     * @param  string  bucketName
     * @param  string  path     目录路径，sdk会补齐末尾的 '/' 
     * @param  int     num      拉取的总数
     * @param  string  pattern  eListBoth,ListDirOnly,eListFileOnly  默认both
     * @param  string  offset   透传字段,用于翻页,前端不需理解,需要往前/往后翻页则透传回来
     * @param  int     order    默认正序(=0), 填1为反序,
     *  
     */
    int listFolder(
        const string &bucketName, 
        const string &path, 
        const int num = 20, 
        const string &pattern = "eListBoth",
        const int order = 0,
        const string &offset = ""
                    );

    /*
     * 前缀搜索
     * @param  string  bucketName
     * @param  string  prefix   列出含此前缀的所有文件
     * @param  int     num      拉取的总数
     * @param  string  pattern  eListBoth,ListDirOnly,eListFileOnly  默认both
     * @param  string  offset   透传字段,用于翻页,前端不需理解,需要往前/往后翻页则透传回来
     * @param  int     order    默认正序(=0), 填1为反序,
     *  
     */
    int prefixSearch(
        const string &bucketName, 
        const string &prefix, 
        const int num = 20, 
        const string &pattern = "eListBoth",
        const int order = 0,
        const string &offset = ""
                    );

    /*
     * 目录信息 updateFolder
     * @param  string  bucketName
     * @param  string  path 路径，sdk会补齐末尾的 '/'
     *
     */
    int updateFolder(
            const string &bucketName, 
            const string &path,
            const string &biz_attr = ""
            );

    /*
     * 文件信息 update
     * @param  string  bucketName
     * @param  string  path 路径
     *
     */
    int update(
            const string &bucketName, 
            const string &path,
            const string &biz_attr = ""
            );

    /*
     * 目录信息 查询
     * @param  string  bucketName
     * @param  string  path 路径，sdk会补齐末尾的 '/' 
     *  
     */
    int statFolder(
            const string &bucketName,
            const string &path
            );

    /*
     * 文件信息 查询
     * @param  string  bucketName
     * @param  string  path 路径
     *  
     */
    int stat(
            const string &bucketName,
            const string &path
            );

    /*
     * 删除目录
     * @param  string  bucketName
     * @param  string  path 路径，sdk会补齐末尾的 '/'
     *
     */
    int delFolder(
            const string &bucketName,
            const string &path
            );

    /*
     * 删除文件
     * @param  string  bucketName
     * @param  string  path 路径，如果是目录则必须以‘/’结尾
     *
     */
    int del(
            const string &bucketName,
            const string &path
            );

    void dump_res();

private:
    string validFolderPath(const string &path);
    string validFilePath(const string &path);
    int upload_prepare(
            const uint64_t fileSize,
            const string &sha,
            const string &sign,
            const string &url,
            const string &bizAttr = "",
            const string &session = "",
            const uint64_t sliceSize = 0
            );

    int upload_data(
            const uint64_t fileSize,
            const string &sha,
            const uint64_t sliceSize,
            const string &sign,
            const string &url,
            const string &srcPath,
            const uint64_t offset,
            const string &session
            );

    int listBase(
        const string &bucketName, 
        const string &path, 
        const int num = 20, 
        const string &pattern = "eListBoth",
        const int order = 0,
        const string &offset = ""
                    );

    int updateBase(
            const string &bucketName, 
            const string &path,
            const string &biz_attr = ""
            );

    int statBase(
            const string &bucketName,
            const string &path
            );

    int delBase(
            const string &bucketName,
            const string &path
            );

protected:
    CURL * _curl_handle;
    uint64_t _timeout;

public:
    string response_str;

    int retCode;
    string retMsg;
    Json::Value retJson;

};


}//namespace Qcloud_cos

#endif
