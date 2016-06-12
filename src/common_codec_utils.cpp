// Copyright (c) 2016, Tencent Inc.
// All rights reserved.
//
// Author: Wu Cheng <chengwu@tencent.com>
// Created: 03/08/2016
// Description:

#include "common_codec_utils.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>

using std::string;

namespace qcloud_cos {
unsigned char CommonCodecUtils::ToHex(const unsigned char &x) {
    return x > 9 ? (x - 10 + 'A') : x + '0';
}

string CommonCodecUtils::UrlEncode(const string &str) {
    string encodedUrl = "";
    std::size_t length = str.length();
    for (size_t i = 0; i < length; ++i) {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~') ||
            (str[i] == '/')) {

            encodedUrl += str[i];
        } else {
            encodedUrl += '%';
            encodedUrl += ToHex((unsigned char)str[i] >> 4);
            encodedUrl += ToHex((unsigned char)str[i] % 16);
        }
    }
    return encodedUrl;
}

string CommonCodecUtils::Base64Encode(const string &plainText) {
    static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    const std::size_t plainTextLen = plainText.size();
    string retval((((plainTextLen + 2) / 3) * 4), '=');
    std::size_t outpos = 0;
    int bits_collected = 0;
    unsigned int accumulator = 0;
    const string::const_iterator plainTextEnd = plainText.end();

    for (string::const_iterator i = plainText.begin(); i != plainTextEnd; ++i) {
        accumulator = (accumulator << 8) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }

    if (bits_collected > 0) {
        assert(bits_collected < 6);
        accumulator <<= 6 - bits_collected;
        retval[outpos++] = b64_table[accumulator & 0x3fu];
    }
    assert(outpos >= (retval.size() - 2));
    assert(outpos <= retval.size());
    return retval;
}

string CommonCodecUtils::HmacSha1(const string &plainText, const string &key) {
    const EVP_MD *engine = EVP_sha1();
    unsigned char *output = (unsigned char *)malloc(EVP_MAX_MD_SIZE);
    unsigned int output_len = 0;

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, (char *)key.c_str(), key.length(), engine, NULL);
    HMAC_Update(&ctx, (unsigned char*)plainText.c_str(), plainText.length());
    HMAC_Final(&ctx, output, &output_len);
    HMAC_CTX_cleanup(&ctx);

    string hmac_sha1_ret((char *)output, output_len);
    free(output);
    return hmac_sha1_ret;
}

string CommonCodecUtils::GetFileSha1(const string &localFilePath) {
    string sha1Digest = "";
    unsigned char buf[8192];
    SHA_CTX sc;
    SHA1_Init(&sc);
    size_t len;
    std::ifstream fileInput(localFilePath.c_str(), std::ios::in | std::ios::binary);
    while(!fileInput.eof()) {
        fileInput.read((char *)buf, sizeof(buf));
        len = fileInput.gcount();
        SHA1_Update(&sc, buf, len);
    }
    fileInput.close();

    unsigned char digestBuf[SHA_DIGEST_LENGTH];
    SHA1_Final(digestBuf, &sc);

    string digestStr = "";
    unsigned char temp_hex;
    for(int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        temp_hex = ToHex(digestBuf[i] / 16);
        digestStr.append(1, temp_hex);
        temp_hex = ToHex(digestBuf[i] % 16);
        digestStr.append(1, temp_hex);
    }
    return digestStr;
}
}
