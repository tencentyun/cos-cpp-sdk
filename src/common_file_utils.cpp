// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/10/2016
// Description:

#include "../include/common_file_utils.h"
#include <string>
#include <fstream>
#include <sstream>

using std::string;

namespace qcloud_cos {

string CommonFileUtils::GetFileContent(const string &localFilePath) {
    std::ifstream fileInput(localFilePath.c_str(), std::ios::in | std::ios::binary);
    std::ostringstream out;

    out << fileInput.rdbuf();
    string content = out.str();

    fileInput.close();
    fileInput.clear();

    return content;
}

uint64_t CommonFileUtils::GetFileLen(const string &localFilePath) {
    std::ifstream fileInput(localFilePath.c_str(), std::ios::in | std::ios::binary);
    fileInput.seekg(0, std::ios::end);
    uint64_t fileLen = fileInput.tellg();
    fileInput.close();
    return fileLen;
}

} // namespace qcloud_cos

