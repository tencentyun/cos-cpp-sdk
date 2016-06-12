function get_root_path_of()
{
    if [ $# -lt 1 ]; then
        echo "ERROR, miss parameter. get_root_path_of some_tag_filename"
        exit 1
    fi
    local tag_filename=$1
    local old_pwd=`pwd`

    # get bash path
    local cur_bash_path=`dirname $BASH_SOURCE`
    local cur_bash_path=`readlink -f ${cur_bash_path}`
    cd ${cur_bash_path}
    while true; do
        if [ -f ${tag_filename} ]; then
            echo `pwd`
            break
        else
            cd ..
        fi
        if [ `pwd` = '/' ]; then
            echo "ERROR: can not find file '${tag_filename}'. "
            break;
        fi
    done
    cd ${old_pwd}
}

cpp_sdk_root=$(get_root_path_of "SDK_ROOT")

echo "cpp_sdk_root=$cpp_sdk_root"
rm "$cpp_sdk_root/include/l5" -rf
rm "$cpp_sdk_root/include/l5_endpoint_provider.h"
rm "$cpp_sdk_root/src/l5_endpoint_provider.cpp"
rm "$cpp_sdk_root/lib/libqos_client.a"
