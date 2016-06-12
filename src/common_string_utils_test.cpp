// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/14/16
// Description:

#include "../include/common_string_utils.h"
#include "gtest/gtest.h"
#include <string>
using std::string;

namespace qcloud_cos{

TEST(CommonStringUtils, IntToString) {
    int num = 300;
    string str = CommonStringUtils::IntToString(300);
    ASSERT_STREQ("300", str.c_str());
}
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
