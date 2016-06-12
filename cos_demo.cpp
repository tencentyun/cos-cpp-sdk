// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: rabbitliu <rabbitliu@tencent.com>
// Created: 06/02/16
// Description:

#include <iostream>
#include <string>

#include "common_string_utils.h"
#include "cos_api.h"

namespace qcloud_cos {

static const uint64_t kAppId = 10022105;
static const std::string kSecretId = "AKIDGFf8r88nYtFdUilQVL1STwDNrrTngcom";
static const std::string kSecretKey = "yM3H3xSREuk96vmBrwh8UadruA5gWhrc";
static const std::string kBucketName = "rabbitliu";

static const std::string kCosSampleDirPath = "/cos_sample_dir/";

// 帮助函数，递归删除目录
bool RecursionDel(CosApi& cos_client,
                  const std::string& bucket_name,
                  const std::string& dir) {
    if (dir[dir.length() - 1] != '/') {
        std::cerr << "dir must end with /" << std::endl;
        return false;
    }
    std::string result = cos_client.ListFolder(kBucketName, dir);
    // std::cout << "result: " << result << std::endl;
    if (result.empty()) {
        std::cerr << "list folder failure" << std::endl;
        return false;
    }

    Json::Value result_json = CommonStringUtils::StringToJson(result);
    if (result_json["code"].asInt() != 0) {
        std::cerr << "List result code is not ok" << std::endl;
        return false;
    }

    Json::Value infos = result_json["data"]["infos"];
    if (!infos.isArray()) {
        std::cerr << "data is not format, infos is not array." << std::endl;
        return false;
    }

    for (int i = 0; i < static_cast<int>(infos.size()); ++i) {
        const Json::Value& sub_value = infos[i];
        std::string path = dir + "/" + sub_value["name"].asString();
        std::cout << "path: " << path << std::endl;
        // 表示一个文件
        if (sub_value.isMember("sha")) {
            result = cos_client.DelFile(bucket_name, path);
            Json::Value tmp_value = CommonStringUtils::StringToJson(result);
            if (tmp_value["code"].asInt() != 0) {
                std::cerr << "del file failure, bucket_name: "
                    << bucket_name << ", file_path: " << path;
                return false;
            }
            // std::cout << "del path: " << path << "successful" << std::endl;
        } else {
            // 表示一个目录，递归删除，infos的path没有/，需手动添加
            RecursionDel(cos_client, bucket_name, path + "/");
        }
    }

    // 最后删掉自己
    result = cos_client.DelFolder(bucket_name, dir);
    Json::Value tmp_value = CommonStringUtils::StringToJson(result);
    if (tmp_value["code"].asInt() != 0) {
        std::cerr << "del folder failure, dir: " << dir << result << "result" << std::endl;
        return false;
    }
    return true;
}

void RecursionDelSample() {
    // 1.构造CosClient，多次构建Client用于测试，用户一个进程建议一个CosAPi类
    CosApiClientOption client_option(kAppId, kSecretId, kSecretKey);
    CosApi cos_client(client_option);
    bool ret = RecursionDel(cos_client, kBucketName, "/rabbit_test_demo/");
    std::cerr << "recursion del result: " << ret << std::endl;
}

void UploadSample() {
    const std::string kUploadTestDir = kCosSampleDirPath + "upload_test_dir_123 456/";
    const std::string kUploadFileName = "libjsoncpp.a";
    const std::string kUploadFilePath = kUploadTestDir + kUploadFileName;
    const std::string kLocalFilePath = "./lib/libjsoncpp.a";

    // 1.构造CosClient，多次构建Client用于测试，用户一个进程建议一个CosAPi类
    CosApiClientOption client_option(kAppId, kSecretId, kSecretKey);
    // 如果从l5获取地址，打开下面注释
    // client_option.l5_modid = 445121;
    // client_option.l5_cmdid = 65536;
    CosApi cos_client(client_option);
    RecursionDel(cos_client, kBucketName, kUploadTestDir);

    // 2.创建用于上传的目录
    CustomOptions options;
    options.AddStringOption("biz_attr", "hello");
    std::string result = cos_client.CreateFolder(kBucketName, kUploadTestDir, options);
    std::cout << "Create folder result: " << result << std::endl;
    result = cos_client.StatFolder(kBucketName, kUploadTestDir);
    std::cout << "Stat folder result: " << result << std::endl;

    // 3.执行上传操作
    CustomOptions upload_options;
    upload_options.AddStringOption("biz_attr", "test 10k");
    result = cos_client.Upload(kLocalFilePath, kBucketName,
                               kUploadFilePath, upload_options);
    std::cout << "Upload file result: " << result << std::endl;

    // 4.查看文件状态
    result = cos_client.StatFile(kBucketName, kUploadFilePath);
    std::cout << "Stat file result: " << result << std::endl;

    // 5.再次上传
    result = cos_client.Upload(kLocalFilePath, kBucketName,
                               kUploadFilePath, upload_options);
    std::cout << "Upload file result: " << result << std::endl;

    // 6.设置覆盖
    upload_options.AddStringOption("insertOnly", "0");
    upload_options.AddStringOption("Content-Type", "text/xml");
    upload_options.AddStringOption("x-cos-meta-rabbitliu", "hello");
    result = cos_client.Upload(kLocalFilePath, kBucketName,
                               kUploadFilePath, upload_options);
    std::cout << "Upload file result: " << result << std::endl;

    // 7.再次查看文件状态
    result = cos_client.StatFile(kBucketName, kUploadFilePath);
    std::cout << "Stat file result: " << result << std::endl;

    // 8.删除文件
    result = cos_client.DelFile(kBucketName, kUploadFilePath);
    std::cout << "Del file result: " << result << std::endl;

    // 10.删除目录
    result = cos_client.DelFolder(kBucketName, kUploadTestDir);
    std::cout << "Del folder result: " << result << std::endl;
}

void MoveFileSample() {
    const std::string kMoveFileTestDir = kCosSampleDirPath + "move_test_dir/";
    const std::string kUploadFileName = "libjsoncpp.a";
    const std::string kRenamedFileName = "libjsoncpp_renamed.a";
    const std::string kUploadFilePath = kMoveFileTestDir + kUploadFileName;
    const std::string kRenamedFilePath = kMoveFileTestDir + kRenamedFileName;
    const std::string kLocalFilePath = "./lib/libjsoncpp.a";

    // 1.构造CosClient，多次构建Client用于测试，用户一个进程建议一个CosAPi类
    CosApiClientOption client_option(kAppId, kSecretId, kSecretKey);
    // 如果从l5获取地址，打开下面注释
    // client_option.l5_modid = 445121;
    // client_option.l5_cmdid = 65536;
    CosApi cos_client(client_option);
    RecursionDel(cos_client, kBucketName, kMoveFileTestDir);

    // 2.创建用于上传的目录
    CustomOptions options;
    options.AddStringOption("biz_attr", "hello");
    std::string result = cos_client.CreateFolder(kBucketName, kMoveFileTestDir, options);
    std::cout << "Create folder result: " << result << std::endl;
    result = cos_client.StatFolder(kBucketName, kMoveFileTestDir);
    std::cout << "Stat folder result: " << result << std::endl;

    // 3.执行上传操作
    CustomOptions upload_options;
    upload_options.AddStringOption("biz_attr", "test");
    result = cos_client.Upload(kLocalFilePath, kBucketName,
                               kUploadFilePath, upload_options);
    std::cout << "Upload file result: " << result << std::endl;

    // 4.查看文件状态
    result = cos_client.StatFile(kBucketName, kUploadFilePath);
    std::cout << "Stat file result: " << result << std::endl;

    // 5.重命名文件
    CustomOptions move_file_options;
    result = cos_client.MoveFile(kBucketName, kUploadFilePath, kRenamedFilePath, move_file_options);
    std::cout << "move file result: " << result << std::endl;
}

} // namespace qcloud_cos

int main(int argc, char** argv) {
    int ret_code = qcloud_cos::COS_Init();
    if (ret_code != 0) {
        std::cerr << "COS_Init error." << std::endl;
        qcloud_cos::COS_UInit();
        return 1;
    }

    // qcloud_cos::UploadSample();
    // qcloud_cos::RecursionDelSample();
    qcloud_cos::MoveFileSample();
    qcloud_cos::COS_UInit();
    return 0;
}
