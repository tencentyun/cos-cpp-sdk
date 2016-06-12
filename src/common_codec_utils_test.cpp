// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/14/16
// Description:

#include "common_codec_utils.h"
#include "gtest/gtest.h"
#include <string>
using std::string;
using std::cout;
using std::endl;

static string plainText = "681805d9f7c6ab988a00c02f1096b1b68a77aaed";
static string hmacKey = "lw7231!2@7g";
namespace qcloud_cos{

TEST(CommonCodecUtils, HmacSha1) {
    string str = CommonCodecUtils::HmacSha1(plainText, hmacKey);
    size_t i = 0;
    size_t len = str.length();
    unsigned int temp_int;
    unsigned char temp_hex;
    for(i = 0; i < len; ++i) {
        temp_int = ((unsigned int)str[i] & 0xff) / 16;
        //cout << "temp_int:" << temp_int << endl;
        temp_hex = CommonCodecUtils::ToHex((unsigned char) temp_int);
        cout << temp_hex;
        //cout << "temp_hex:" << temp_hex << endl;
        temp_int = (unsigned int)str[i] % 16;
        temp_hex = CommonCodecUtils::ToHex((unsigned char) temp_int);
        cout << temp_hex;
    }
    cout << endl;
}
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
