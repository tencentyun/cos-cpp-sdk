#include "Auth.h"
#include "Conf.h"

using namespace std;
using namespace Tencentyun;

int main () {

	string sign = Auth::appSign_more(123, "test_bucket");
	cout << "sign:" << sign << endl;
	return 0;
}
