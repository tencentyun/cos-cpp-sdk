#ifndef _TENCENTYUN_COS_COSAPI_H_H_
#define _TENCENTYUN_COS_COSAPI_H_H_
#pragma once

#include <stdint.h>
#include <string>
#include <map>

#include "cos_api_defines.h"

namespace qcloud_cos {

/// 防止反复Init
extern bool g_init;
/// @brief cos-cpp-sdk的Init函数，用于main函数，初始化cos sdk相关库
/// @return 0代表成功，否则初始化失败
int COS_Init();

/// @brief cos-cpp-sdk的Init函数，用于main函数退出时的清理工作
void COS_UInit();

class CosApi {
public:
    /**
     * @brief 构造函数
     *
     * @param option 填充appid secretid secretkey timeout
     */
    explicit CosApi(const CosApiClientOption& option);

    /**
     * @brief 析构函数
     */
    virtual ~CosApi();

    /**
     * @brief 上传文件, 适用于小文件(8MB以下)上传
     *
     * @param  std::string  srcPath     本地文件路径
     * @param  std::string  bucketName  上传的bcuket名称
     * @param  std::string  dstPath     上传的文件路径
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string Upload(
        const std::string& srcPath,
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options = DefaultUploadOptions()
    );

    /**
     * @brief  串行分片上传文件
     *
     * @param  std::string  srcPath     本地文件路径
     * @param  std::string  bucketName  上传的bcuket名称
     * @param  std::string  dstPath     上传的远程文件路径
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string UploadSlice(
        const std::string& srcPath,
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options = DefaultUploadSliceOptions()
    );


    /**
     * @brief 在Cos上创建文件夹
     *
     * @param string bucketName     上传的bucket名称
     * @param string dstPath        创建的目录的远程路径
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string CreateFolder(
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options = CustomOptions()
    );

    /**
     * @brief 查询目录列表
     *
     * @param  std::string  bucketName     上传的bucket名称
     * @param  std::string  dstPath     目录路径
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功, data字段包含目录列表
     */
    std::string ListFolder(
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options = DefaultListOptions()
    );

    /**
     * @brief 进行前缀搜索
     *
     * @param  std::string  bucketName     上传的bucket名称
     * @param  std::string  prefix   列出含此前缀的所有文件
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功, data字段包含结果列表
     */
    std::string PrefixSearch(
        const std::string& bucketName,
        const std::string& prefix,
        const CustomOptions& options = DefaultListOptions()
    );

    /**
     * @brief 目录信息 UpdateFolder
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  dstPath       Cos目录路径，必须是合法的目录路径
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string UpdateFolder(
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options = CustomOptions()
    );

    /**
     * @brief  更新文件属性
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  dstPath       Cos文件路径, 必须是合法的文件路径
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string UpdateFile(
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options = CustomOptions()
    );

    /**
     * @brief  查询目录属性等信息
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  dstPath       Cos目录路径
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string StatFolder(
        const std::string& bucketName,
        const std::string& dstPath
    );

    /**
     * @brief 查询文件属性等信息
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  dstPath       Cos文件路径
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string StatFile(
        const std::string& bucketName,
        const std::string& dstPath
    );

    /**
     * @brief 删除目录
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  dstPath       Cos目录路径, 必须是合法的目录路径
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string DelFolder(
        const std::string& bucketName,
        const std::string& dstPath
    );

    /**
     * @brief 删除文件
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  dstPath       Cos文件路径, 必须是合法的文件路径
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string DelFile(
        const std::string& bucketName,
        const std::string& dstPath
    );

    /**
     * @brief 重命名文件
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  srcPath       Cos文件路径, 必须是合法的文件路径
     * @param  std::string  dstPath       Cos文件路径, 必须是合法的文件路径
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string MoveFile(
        const std::string& bucketName,
        const std::string& srcPath,
        const std::string& dstPath,
        const CustomOptions& options = DefaultRenameFileOptions()
    );

private:
    /**
     * @brief 分片上传文件
     *
     * @param url            完整的Cos URL
     * @param fileSize       要发送的文件大小
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     * @param parallel       是否要并发
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string UploadSliceInternal(
        const std::string& srcPath,
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options,
        bool parallel
    );

    /**
     * @brief 分片上传时的第一步，发送控制分片获取信息
     *
     * @param url            完整的Cos URL
     * @param sign           签名字符串
     * @param http_params    http 参数
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string UploadSliceCmd(
        const std::string& url,
        const std::string sign,
        const std::map<std::string, std::string>& http_params
    );

    /**
     * @brief 分片上传的第二部，发送数据分片
     *
     * @param url              完整的COS URL
     * @param sign             签名
     * @param session          session值
     * @param offset           分片偏移量
     * @param sliceContent     分片的内容
     * @param sliceLen         分片大小
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string UploadSliceData(
        const std::string& url,
        const std::string& sign,
        const std::string& session,
        const uint64_t offset,
        const unsigned char *sliceContent,
        const uint64_t sliceLen);

    /**
     * @brief  罗列目录列表, prefixSearch和listFolder调用
     *
     * @param  std::string  bucketName    bucket名称
     * @param  std::string  dstPath       目录路径，sdk会补齐末尾的 '/'
     * @param  CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功, data字段包含结果列表
     */
    std::string ListBase(
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options
    );

    /**
     * @brief 更新文件或目录属性, 由updateFile和updateFolder调用
     *
     * @param string bucketName         bucket名称
     * @param string dstPath            cos远程目录名称
     * @param CustomOptions options  用户自定义的参数,参数说明详见文档
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string UpdateBase(
        const std::string& bucketName,
        const std::string& dstPath,
        const CustomOptions& options
    );

    /**
     * @brief 查询文件或者目录属性，由statFile和statFolder调用
     *
     * @param string bucketName         bucket名称
     * @param string dstPath            cos远程目录名称
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string StatBase(
        const std::string& bucketName,
        const std::string& dstPath
    );

    /**
     * @brief 删除文件或者目录，由delFile和delFolder调用
     *
     * @param string bucketName         bucket名称
     * @param string dstPath            cos远程目录名称
     *
     * @return Json样式的字符串, 其中成员code为0表示成功
     */
    std::string DelBase(
        const std::string& bucketName,
        const std::string& dstPath
    );

    void FillHttpParams(const CustomOptions& options,
                        std::map<std::string, std::string>* http_params
    );

    std::string GetEncodedCosUrl(const std::string& bucket_name,
                                 const std::string& path,
                                 const CosApiClientOption& options);

    CosApiClientOption m_client_option;
};
} //namespace qcloud_cos

#endif // _TENCENTYUN_COS_COSAPI_H_H_
