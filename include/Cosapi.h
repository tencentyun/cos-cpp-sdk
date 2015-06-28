#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include "curl/curl.h"

#include "Auth.h"

#ifndef _TENCENTYUN_COS_COSAPI_H_H_
#define _TENCENTYUN_COS_COSAPI_H_H_

using namespace std;

namespace Tencentyun {

class Cosapi
{

public:
    static const string API_COSAPI_END_POINT;

	static const int EXPIRED_SECONDS;
	static const int DEFAULT_SLICE_SIZE;
	static const int MIN_SLICE_FILE_SIZE;
	
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

	Cosapi(
        const uint64_t appid, 
		const string &secretId,
		const string &secretKey);
	virtual ~Cosapi();

	int sendRequest(
			const string &url,
			const int isPost,
			const vector<string> *headers);

	string generateResUrl(
			const string &bucketName,
			const string &dstPath);

    /*
     * 目录列表,前缀搜索
     * @param  string  bucketName
     * @param  string  path     目录路径 web.file.myqcloud.com/files/v1/[appid]/[bucket_name]/[DirName]/
     *                           web.file.myqcloud.com/files/v1/appid/[bucket_name]/[DirName]/[prefix] <- 如果填写prefix, 则列出含此前缀的所有文件
     * @param  string  pattern  eListBoth,ListDirOnly,eListFileOnly  默认both
     * @param  string  offset   透传字段,用于翻页,前端不需理解,需要往前/往后翻页则透传回来
     * @param  int     num      拉取的总数
     * @param  int     order    默认正序(=0), 填1为反序,
     *  
     */
    int list(
                    const string &bucketName, 
					const string &path, 
					const int num = 20, 
                    const string &pattern = "eListBoth",
					const string &offset = "",
					const int order = 0
					);

protected:
	CURL * _curl_handle;

public:
	string response_str;

};


}//namespace Tencentyun

#endif
