#include "cstdio"
#include <cstring>

#include "Cosapi.h"

namespace Tencentyun {

const string Cosapi::API_COSAPI_END_POINT = 
	"http://web.file.myqcloud.com/files/v1/";

size_t post_callback( 
		char *ptr, 
		size_t size, 
		size_t nmemb, 
		void *userdata) {

	Cosapi * cosapiPtr = 
		static_cast<Cosapi*>(userdata);
	size_t byte_num = size * nmemb;
	char tmp[byte_num+1];
	memcpy(tmp, ptr, byte_num);
	tmp[byte_num] = 0;
	cosapiPtr->response_str += tmp;
	return byte_num;
}


const int Cosapi::EXPIRED_SECONDS = 2592000;
const int Cosapi::DEFAULT_SLICE_SIZE = 3145728;
const int Cosapi::MIN_SLICE_FILE_SIZE = 10485760;

int32_t Cosapi::global_init() {
    CURLcode retCode;
    retCode = curl_global_init(CURL_GLOBAL_ALL);
	if (CURLE_OK != retCode){
	    return -1;
	}
	return 0;
}

void Cosapi::global_finit() {
    curl_global_cleanup();
	return;
}

Cosapi::Cosapi(
        const uint64_t appid, 
		const string &secretId,
		const string &secretKey)
	        :APPID(appid),
            SECRET_ID(secretId),
            SECRET_KEY(secretKey) {

    _curl_handle = curl_easy_init();
}

Cosapi::~Cosapi() {
	curl_easy_cleanup(_curl_handle);
}

string Cosapi::generateResUrl(
		const string &bucketName,
		const string &dstPath) {

    string url = "";
	char urlBytes[10240];
	snprintf(urlBytes, sizeof(urlBytes),
			"%s%lu/%s%s",
			API_COSAPI_END_POINT.c_str(),
			APPID, 
			bucketName.c_str(), 
			dstPath.c_str());

	url = urlBytes;
	return url;
}

int Cosapi::sendRequest(
		const string &url,
		const int isPost,
		const vector<string> *headers) {

    curl_easy_setopt(
            _curl_handle, CURLOPT_URL, url.c_str());
    if (isPost) {
		curl_easy_setopt(
            _curl_handle, CURLOPT_POST, 1);
	}

	struct curl_slist *list = NULL;
	if (headers) {
		vector<string>::const_iterator it = 
			headers->begin();
		while (it != headers->end()) {
            list = curl_slist_append(
					list, it->c_str());
			it++;
		}
		curl_easy_setopt(
				_curl_handle, CURLOPT_HTTPHEADER, list);

	}

	curl_easy_setopt(
			_curl_handle, CURLOPT_WRITEFUNCTION, 
			post_callback);
	curl_easy_setopt(
			_curl_handle, CURLOPT_WRITEDATA, 
			(void*)this );

	curl_easy_perform(_curl_handle);
	curl_slist_free_all(list);

	cout << "Response: " << response_str << endl;
	return 0;
}

int Cosapi::list(
                    const string &bucketName, 
					const string &path, 
					const int num, 
                    const string &pattern,
					const string &offset,
					const int order 
		) {

	uint64_t expired = time(NULL) + EXPIRED_SECONDS;
	string url = generateResUrl(bucketName, path); 

	char queryStr[1024];
	snprintf(queryStr, sizeof(queryStr),
			"?op=list&num=%d", num);

	url += queryStr;

	string sign = 
		Auth::appSign_more(
				APPID, SECRET_ID, SECRET_KEY,
				expired, bucketName);

	vector<string> headers;
	headers.push_back("Authorization: " + sign);
	headers.push_back("Content-Type: application/json");

	int ret =
		sendRequest(url, 0, &headers);

    return ret;
}

}//namespace Tencentyun
