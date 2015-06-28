#include "Cosapi.h"

using namespace std;
using namespace Tencentyun;

int main () {

    Cosapi::global_init();
    Cosapi api(1000029,
            "AKID4EAND9RuE6psJYOuFKlh0Jeg9Q9BmmS2",
            "jvAAGz07ElrJF1oKWpbKhAIzWF5W6BZN");
    
    //test sign
    /*
    string sign = Auth::appSign_more(
            1000029, "AKID4EAND9RuE6psJYOuFKlh0Jeg9Q9BmmS2",
            "jvAAGz07ElrJF1oKWpbKhAIzWF5W6BZN",
            123, "test_bucket");
    cout << "sign:" << sign << endl;
    */

    //test createFolder
    /*
    api.createFolder(
            "test_mikenwang_20150623", "/test/");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;
    */

    //test update
    /*
    api.update(
            "test_mikenwang_20150623", "/test/", "test update biz_attr");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;
    */

    //test stat
    /*
    api.stat("test_mikenwang_20150623", "/chrome_200_percent.pak");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;
    */

    //test delete
    /*
    api.del(
            "test_mikenwang_20150623", "/test/");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;
    */

    //test list
    /*
    api.list("test_mikenwang_20150623", "/", 10);
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;
    */

    //test upload
    /*
    api.upload(
            //"../test.mp4", "test_mikenwang_20150623", 
            //"/test.mp4");
            "/home/ubuntu/cos/sdk/63MB_test.exe", "test_mikenwang_20150623", 
            "/63MB_test.exe");
            //"/home/ubuntu/cos/sdk/1.6GB_test.mkv", "test_mikenwang_20150623", 
            //"/1.6GB_test.mkv");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;
    */

    //test upload_slice
    ///*
    api.upload_slice(
            //"../test.mp4", "test_mikenwang_20150623", 
            //"/test.mp4");
            //"/home/ubuntu/cos/sdk/63MB_test.exe", "test_mikenwang_20150623", 
            //"/63MB_test.exe", "", 2*1024*1024);
            "/home/ubuntu/cos/sdk/1.6GB_test.mkv", "test_mikenwang_20150623", 
            "/1.6GB_test.mkv", "", 3*1024*1024);
    //*/

    Cosapi::global_finit();
    return 0;
}
