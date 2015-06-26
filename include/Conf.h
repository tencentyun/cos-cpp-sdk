#ifndef _TENCENTYUN_COS_CONF_H_H_
#define _TENCENTYUN_COS_CONF_H_H_

#include <iostream>
#include <string>
#include <stdint.h>
using namespace std;

namespace Tencentyun {

class Conf
{
public:
    static const string API_COSAPI_END_POINT;

	static const uint64_t APPID;
    static const string SECRET_ID;
    static const string SECRET_KEY;
};

}//namespace Tencentyun

#endif
