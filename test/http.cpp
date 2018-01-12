//
// Created by lby on 2018/1/11.
//

#include <orz/net/http.h>
#include <orz/utils/log.h>

int main(int argc, char *argv[])
{
    orz::URL url("http://www.baidu.com/index.html");
    ORZ_LOG(orz::INFO) << "URL: " << url.url();
    ORZ_LOG(orz::INFO) << "protocol: " << url.protocol();
    ORZ_LOG(orz::INFO) << "host: " << url.host();
    ORZ_LOG(orz::INFO) << "port: " << url.port();
    ORZ_LOG(orz::INFO) << "target: " << url.target();
    ORZ_LOG(orz::INFO) << "valid: " << std::boolalpha << url.valid();
    orz::http_request(url, orz::http::GET, "");
    return 0;
}