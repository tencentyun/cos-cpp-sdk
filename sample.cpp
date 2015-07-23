#include "Cosapi.h"
#include "cstring"

using namespace std;
using namespace Qcloud_cos;

int main () {

    string bucketName = "test_mikenwang_20150623";

    Cosapi::global_init();
    Cosapi api(1000029,
            "AKID4EAND9RuE6psJYOuFKlh0Jeg9Q9BmmS2",
            "jvAAGz07ElrJF1oKWpbKhAIzWF5W6BZN", 30);
    
    //test sign
    /*
    string sign = Auth::appSign(
            1000029, "AKID4EAND9RuE6psJYOuFKlh0Jeg9Q9BmmS2",
            "jvAAGz07ElrJF1oKWpbKhAIzWF5W6BZN",
            123, bucketName);
    cout << "sign:" << sign << endl;
    */

    //test createFolder
    /*
    api.createFolder(
            bucketName, "/test/");
    api.dump_res();
    */

    //test listFolder
    ///*
    api.listFolder(
            bucketName, "/", 10);
    api.dump_res();
    //*/

    //test prefixSearch
    /*
    api.prefixSearch(
            bucketName, "/test", 10);
    api.dump_res();
    */

    //test updateFolder
    /*
    api.updateFolder(
            bucketName, "/test/", "0");
    api.dump_res();
    */

    //test update
    /*
    api.update(
            bucketName, "/test.log", "");
    api.dump_res();
    */

    //test statFolder
    /*
    api.statFolder(
            bucketName, "/test/");
    api.dump_res();
    */

    //test stat
    /*
    api.stat(
            bucketName, "/test.log");
    api.dump_res();
    */

    //test deleteFolder
    /*
    api.delFolder(
            bucketName, "/test/");
    api.dump_res();
    */

    //test delete
    /*
    api.del(
            bucketName, "/test.log");
    api.dump_res();
    */

    //test upload
    /*
    api.upload(
            "../test.log", bucketName, 
            "/test.log");
    api.dump_res();
    */

    //test upload_slice
    /*
    api.upload_slice(
            "../test.log", bucketName, 
            "/test.log");
     */

    Cosapi::global_finit();
    return 0;
}
