#ifndef _TENCENTYUN_COS_AUTH_H_H_
#define _TENCENTYUN_COS_AUTH_H_H_

#include <iostream>
#include <string>
#include <stdint.h>
using namespace std;

namespace Qcloud_cos{

class Auth
{
    static const int AUTH_URL_FORMAT_ERROR;
    static const int AUTH_SECRET_ID_KEY_ERROR;
public:

    /**
     * 生成多次有效签名函数（用于上传和下载资源，有效期内可重复对不同资源使用）
     * @param  uint64_t expired    过期时间,unix时间戳  
     * @param  string   bucketName 文件所在bucket
     *
     * @return string   sign   签名
     */
    static string appSign(
            const uint64_t appId, 
            const string &secretId,
            const string &secretKey,
            const uint64_t expired,
            const string &bucketName);

     /**
     * 生成单次有效签名函数（用于删除和更新指定path的资源，使用一次即失效）
     * @param  string   path     文件路径，以 '/' 开头
     * @param  string   bucketName 文件所在bucket
     *
     * @return string   sign   签名
     */
    static string appSign_once(
            const uint64_t appId, 
            const string &secretId,
            const string &secretKey,
            const string &path,
            const string &bucketName);

private:
    /**
     * 签名函数（上传、下载、查看资源需要多次有效签名，
     *           更新、删除资源需要单次有效签名）
     * @param  uint64_t appId
     * @param  string   secretId
     * @param  string   secretKey
     * @param  uint64_t expired       过期时间,unix时间戳
     * @param  string   fileId     资源全路径，格式为 /{$appId}/{$bucketName}/${path}
     * @param  string   bucketName 文件所在bucket
     *
     * @return string   sign   签名
     */

    static string appSignBase(
            const uint64_t appId, 
            const string &secretId,
            const string &secretKey,
            const uint64_t expired,
            const string &fileId,
            const string &bucketName); 


};

}//namespace Qcloud_cos

#endif
