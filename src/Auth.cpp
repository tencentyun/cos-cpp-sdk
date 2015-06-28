#include "Auth.h"
#include <cstring>
#include <openssl/hmac.h>

namespace Tencentyun{

const int Auth::AUTH_URL_FORMAT_ERROR = -1;
const int Auth::AUTH_SECRET_ID_KEY_ERROR = -2;

int HmacEncode(const char * algo,  
                const char * key, unsigned int key_length,  
                const char * input, unsigned int input_length,  
                unsigned char * &output, unsigned int &output_length) {  
        const EVP_MD * engine = NULL;  
        if(strcasecmp("sha512", algo) == 0) {  
                engine = EVP_sha512();  
        }  
        else if(strcasecmp("sha256", algo) == 0) {  
                engine = EVP_sha256();  
        }  
        else if(strcasecmp("sha1", algo) == 0) {  
                engine = EVP_sha1();  
        }  
        else if(strcasecmp("md5", algo) == 0) {  
                engine = EVP_md5();  
        }  
        else if(strcasecmp("sha224", algo) == 0) {  
                engine = EVP_sha224();  
        }  
        else if(strcasecmp("sha384", algo) == 0) {  
                engine = EVP_sha384();  
        }  
        else if(strcasecmp("sha", algo) == 0) {  
                engine = EVP_sha();  
        }  
        else {  
                cout << "Algorithm " << algo << " is not supported by this program!" << endl;  
                return -1;  
        }  
  
        output = (unsigned char*)malloc(EVP_MAX_MD_SIZE);  
  
        HMAC_CTX ctx;  
        HMAC_CTX_init(&ctx);  
        HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);  
        HMAC_Update(&ctx, (unsigned char*)input, strlen(input));        // input is OK; &input is WRONG !!!  
  
        HMAC_Final(&ctx, output, &output_length);  
        HMAC_CTX_cleanup(&ctx);  
  
        return 0;  
}  

static const std::string base64_chars =   
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"  
             "abcdefghijklmnopqrstuvwxyz"  
             "0123456789+/";  
  
  
static inline bool is_base64(unsigned char c) {  
  return (isalnum(c) || (c == '+') || (c == '/'));  
}  
  
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {  
  std::string ret;  
  int i = 0;  
  int j = 0;  
  unsigned char char_array_3[3];  
  unsigned char char_array_4[4];  
  
  while (in_len--) {  
    char_array_3[i++] = *(bytes_to_encode++);  
    if (i == 3) {  
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;  
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);  
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);  
      char_array_4[3] = char_array_3[2] & 0x3f;  
  
      for(i = 0; (i <4) ; i++)  
        ret += base64_chars[char_array_4[i]];  
      i = 0;  
    }  
  }  
  
  if (i)  
  {  
    for(j = i; j < 3; j++)  
      char_array_3[j] = '\0';  
  
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;  
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);  
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);  
    char_array_4[3] = char_array_3[2] & 0x3f;  
  
    for (j = 0; (j < i + 1); j++)  
      ret += base64_chars[char_array_4[j]];  
  
    while((i++ < 3))  
      ret += '=';  
  
  }  
  
  return ret;  
  
}  

/**
 * 签名函数（上传、下载、查看资源会生成多次有效签名，
 *           更新、删除资源会生成单次有效签名）
 * @param  uint64_t appId
 * @param  string   secretId
 * @param  string   secretKey
 * @param  uint64_t expired       过期时间,unix时间戳
 * @param  string   fileId     文件路径，以 /{$appId}/{$bucketName} 开头
 * @param  string   bucketName 文件所在bucket
 *
 * @return string   sign   签名
 */

string Auth::appSign(
	const uint64_t appId, 
	const string &secretId,
	const string &secretKey,
	const uint64_t expired,
	const string &fileId,
	const string &bucketName) {

	if (secretId.empty() || secretKey.empty()) {
		return "";
	}

	time_t now = time(NULL);
	unsigned int seed = now;
	uint64_t rdm = rand_r(&seed);
	char plainText[10240];
	unsigned char * output = NULL;
	unsigned int output_len = 0;

	unsigned int input_length = snprintf(plainText, 10240, 
			"a=%lu&k=%s&e=%lu&t=%lu&r=%lu&f=%s&b=%s",
			appId, secretId.c_str(), expired,
			now, rdm, fileId.c_str(), bucketName.c_str());

	int ret = HmacEncode("SHA1", 
			secretKey.c_str(), secretKey.length(),
			plainText, input_length,
		    output, output_len);

	if (ret != 0) {
		return "";
	}

	unsigned char bin[10240];
    unsigned int bin_len = 0;
	memcpy(bin, output, output_len);
	memcpy(bin + output_len, plainText, input_length);
	bin_len = output_len + input_length;
	free(output);

	string sign = base64_encode(bin, bin_len);

	return sign;
} 


 /**
 * 生成单次有效签名函数（用于删除和更新指定fileId资源，使用一次即失效）
 * @param  string   fileId     文件路径，以 /{$appId}/{$bucketName} 开头
 * @param  string   bucketName 文件所在bucket
 *
 * @return string   sign   签名
 */
string Auth::appSign_once(
	const uint64_t appId, 
	const string &secretId,
	const string &secretKey,
	const string &fileId,
	const string &bucketName) {

    return appSign(
			appId, secretId, secretKey, 
			0, fileId, bucketName);;
}

/**
 * 生成多次有效签名函数（用于上传和下载资源，有效期内可重复对不同资源使用）
 * @param  uint64_t expired    过期时间,unix时间戳  
 * @param  string   bucketName 文件所在bucket
 *
 * @return string   sign   签名
 */
string Auth::appSign_more(
	const uint64_t appId, 
	const string &secretId,
	const string &secretKey,
	const uint64_t expired,
	const string &bucketName) {

	return appSign(
			appId, secretId, secretKey, 
			expired, "", bucketName);;
}



}//namespace Tencentyun
