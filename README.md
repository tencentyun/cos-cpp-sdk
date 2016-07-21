# cos-cpp-sdk

#linux等类UINX系统使用手册
##需要安装的库和工具
openssl: ubuntu下运行 sudo apt-get install libssl-dev 安装  
其他类uinx系统按照各自的方法安装好该库  


curl: 在 Ubuntu 12.04.2 LTS,linux 版本 3.13.0-32-generic，编译了.a放在lib目录下。  
      如果有问题，在http://curl.haxx.se/download/curl-7.43.0.tar.gz 下载源码，  
      编译生成 .a 或者 .so 放到 lib目录下，替换掉原来的libcurl.a  

jsoncpp： 在 Ubuntu 12.04.2 LTS,linux 版本 3.13.0-32-generic，编译了.a放在lib目录下。  
      如果有问题，在 https://github.com/open-source-parsers/jsoncpp 下载源码，  
      编译生成 .a 或者 .so 放到 lib目录下，替换掉原来的libjsoncpp.a  

cmake: 去http://www.cmake.org/download/ 下载cmake安装好即可  

##编译生成静态库.a
执行下面的命令  
cd ${cos-cpp-sdk}  
mkdir -p build  
cd build  
cmake ..  
make  

需要将cos_demo.cpp里的appid、secretId、secretKey、bucket等信息换成你自己的。  
生成的cos_demo就可以直接运行，试用，  

生成的静态库，名称为:libcosdk.a  

##将生成的库链接进自己的项目
生成的libcosdk.a放到你自己的工程里lib路径下，  
include目录下的 auth_utility.h  cosapi.h curl  json  openssl都放到你自己的工程的include路径下。  

例如我的项目里只有一个cos_demo.cpp,项目目录和sdk在同级目录，copy libcosdk.a 到项目所在目录  
那么编译命令为:  
g++ -o cos_demo cos_demo.cpp -I ./include/ -L. -L../cos-cpp-sdk/lib/ -lcosdk -lcurl -lcrypto -lssl -lrt -ljsoncpp

#windows系统暂时不支持

#sample例子
使用接口前，必须调用：  
    qcloud_cos::COS_Init();  
	CosApiClientOption client_option("your appid", "your secretId", "your secretKey", "interface timeout");
    Cosapi api(client_option);

注意cos上的path以 / 开头

##计算多次签名，静态函数任何地方可以直接调用
    string sign = AuthUtility::AppSignMuti(
                        "your appid", "your secretId",
                        "your secretKey",
                        "expired unix timestamp", 
                        "bucketName");

##创建目录
    api.CreateFolder(
                "bucketName", "/test/");

##listFolder目录下文件列表
    api.ListFolder("bucketName", "/", 10);

##prefixSearch前缀搜索
    api.PrefixSearch("bucketName", "/test", 10);

##更新目录属性
    api.UpdateFolder(
            bucketName, "/test/", "attr");

##更新文件属性
    api.Update(
            bucketName, "/test.log", "attr");

##statFolder查询目录
    api.StatFolder(
            bucketName, "/test/");

##stat查询文件
    //可以用来判断文件是否存在
    api.Stat(
            bucketName, "/test.log");

##删除目录
    api.DeleteFolder(
            "bucketName", "/test/");

##删除文件
    api.DelFile(
            "bucketName", "/test.log");

##上传文件
    api.Upload(
            "srcPath", "bucketName",-
            "/dstPath");

##大文件分片上传
    //sliceSize参数可以指定分片大小，默认是 512KB
    //后台允许的最大分片大小是3MB
    //如果中途失败，以相同的参数再次调用upload_slice可以自动断点续传
    api.UploadSlice(
            "srcPath", "bucketName",-
            "/dstPath", "", 3*1024*1024);

