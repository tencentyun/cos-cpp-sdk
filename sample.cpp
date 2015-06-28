#include "Cosapi.h"

using namespace std;
using namespace Tencentyun;

int main () {

	Cosapi::global_init();
	
	Cosapi api(1000029, "AKID4EAND9RuE6psJYOuFKlh0Jeg9Q9BmmS2",
			"jvAAGz07ElrJF1oKWpbKhAIzWF5W6BZN");

	string sign = Auth::appSign_more(
			1000029, "AKID4EAND9RuE6psJYOuFKlh0Jeg9Q9BmmS2",
			"jvAAGz07ElrJF1oKWpbKhAIzWF5W6BZN",
			123, "test_bucket");
	cout << "sign:" << sign << endl;

	api.list("test_mikenwang_20150623", "/", 10);

	Cosapi::global_finit();
	return 0;
}
