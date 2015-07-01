# cos-cpp-sdk

#linux等类UINX系统使用手册
##需要安装的库和工具
openssl: ubuntu下运行 sudo apt-get install libssl-dev 安装＜/br＞
      其他类uinx系统按照各自的方法安装好该库＜/br＞
curl: 在 Ubuntu 12.04.2 LTS,linux 版本 3.13.0-32-generic，编译了.a放在lib目录下。＜/br＞
      如果有问题，在http://curl.haxx.se/download/curl-7.43.0.tar.gz下载源码，＜/br＞
      编译生成 .a 或者 .so 放到 lib目录下，替换掉原来的libcurl.a＜/br＞
jsoncpp： 在 Ubuntu 12.04.2 LTS,linux 版本 3.13.0-32-generic，编译了.a放在lib目录下。＜/br＞
      如果有问题，在 https://github.com/open-source-parsers/jsoncpp 下载源码，＜/br＞
      编译生成 .a 或者 .so 放到 lib目录下，替换掉原来的libjsoncpp.a＜/br＞
cmake: 去http://www.cmake.org/download/下载cmake安装好即可＜/br＞

##编译生成静态库.a
执行下面的命令＜/br＞
cd ${cos-cpp-sdk}＜/br＞
mkdir -p build＜/br＞
cd build＜/br＞
cmake ..＜/br＞
make＜/br＞

需要将sample.cpp里的appid、secretId、secretKey、bucket等信息换成你自己的。＜/br＞
生成的sample就可以直接运行，试用，＜/br＞

生成的静态库，名称为:libcosdk.a＜/br＞

##将生成的库链接进自己的项目
生成的libcosdk.a放到你自己的工程里lib路径下，＜/br＞
include目录下的 Auth.h  Cosapi.h curl  json  openssl都放到你自己的工程的include路径下。＜/br＞

例如我的项目里只有一个sample.cpp,项目目录和sdk在同级目录，copy libcosdk.a 到项目所在目录＜/br＞
那么编译命令为:＜/br＞
g++ -o sample sample.cpp -I ./include/ -L. -L../cos-cpp-sdk/lib/ -lcosdk -lcurl -lcrypto -lssl -lrt -ljsoncpp

#windows系统咱不支持

#sample例子
使用接口前，必须调用：＜/br＞
Cosapi::global_init();＜/br＞
Cosapi api("your appid",
                "your secretId",
                "your secretKey");

注意cos上的path以 / 开头

##计算多次签名，静态函数任何地方可以直接调用
    string sign = Auth::appSign_more(
                        "your appid", "",
                        "your secretId",
                        123, "bucketName");

##创建目录
    //注意path以 / 结尾
    api.createFolder(
                "bucketName", "/test/");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;

##删除文件或者目录，目录必须以 / 结尾
    api.del(
            "bucketName", "/test/");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;

##list文件列表
    api.list("bucketName", "/", 10);
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;

##上传文件
    api.upload(
            "srcPath", "bucketName",-
            "/dstPath");
    cout << "retJson:" << api.retJson << endl;
    cout << "retCode:" << api.retCode << endl;
    cout << "retMsg:" << api.retMsg << endl;

##大文件分片上传
    //sliceSize参数可以指定分片大小，默认是 512KB
    //后台允许的最大分片大小是3MB
    //如果中途失败，以相同的参数再次调用upload_slice可以自动断点续传
    api.upload_slice(
            "srcPath", "bucketName",-
            "/dstPath", "", 3*1024*1024);

