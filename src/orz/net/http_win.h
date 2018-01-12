//
// Created by lby on 2018/1/11.
//

#ifndef ORZ_NET_HTTP_WIN_H
#define ORZ_NET_HTTP_WIN_H

#include "orz/net/http.h"
#include "orz/utils/log.h"
#include "orz/mem/need.h"

#include <windows.h>
#include <wininet.h>

#include "orz/io/jug/binary.h"

namespace orz {

    int defulat_protocol_port(const std::string &protocal, int port)
    {
        if (port > 0) return port;
        std::string local_protocal = protocal;
        for (auto &ch : local_protocal) ch = std::tolower(ch);
        if (local_protocal == "http") return INTERNET_DEFAULT_HTTP_PORT;
        if (local_protocal == "https") return INTERNET_DEFAULT_HTTPS_PORT;
        return 0;
    }

    const char *verb_string(http::VERB verb) {
        switch (verb) {
            case http::GET:
                return "GET";
            case http::POST:
                return "POST";
            default:
                return "GET";
        }
    }

    DWORD default_http_flags() {
        return //flags common to open functions
                INTERNET_FLAG_RELOAD |
                //flags
                //INTERNET_FLAG_ASYNC |
                //additional cache flags
                //INTERNET_FLAG_DONT_CACHE |
                //additional flags
                //INTERNET_FLAG_SECURE |//启用ssl模式
                INTERNET_FLAG_KEEP_CONNECTION |
                INTERNET_FLAG_NO_AUTO_REDIRECT |
                INTERNET_FLAG_READ_PREFETCH |
                INTERNET_FLAG_NO_COOKIES |
                INTERNET_FLAG_NO_AUTH |
                INTERNET_FLAG_RESTRICTED_ZONE |
                INTERNET_FLAG_CACHE_IF_NET_FAIL |
                //ssl ignore flags
                INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |//忽略ssl模式下的https->http跳转
                INTERNET_FLAG_IGNORE_CERT_CN_INVALID |//忽略ssl模式下的证书名称错误
                //more cache flags
                INTERNET_FLAG_NO_UI |
                0;
    }

    DWORD default_https_flags() {
        return //flags common to open functions
                INTERNET_FLAG_RELOAD |
                //flags
                //INTERNET_FLAG_ASYNC |
                //additional cache flags
                //INTERNET_FLAG_DONT_CACHE |
                //additional flags
                INTERNET_FLAG_SECURE |//启用ssl模式
                INTERNET_FLAG_KEEP_CONNECTION |
                INTERNET_FLAG_NO_AUTO_REDIRECT |
                INTERNET_FLAG_READ_PREFETCH |
                INTERNET_FLAG_NO_COOKIES |
                INTERNET_FLAG_NO_AUTH |
                INTERNET_FLAG_RESTRICTED_ZONE |
                INTERNET_FLAG_CACHE_IF_NET_FAIL |
                //ssl ignore flags
                INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |//忽略ssl模式下的https->http跳转
                INTERNET_FLAG_IGNORE_CERT_CN_INVALID |//忽略ssl模式下的证书名称错误
                //more cache flags
                INTERNET_FLAG_NO_UI |
                0;
    }

    DWORD defulat_protocol_flags(const std::string &protocal)
    {
        std::string local_protocal = protocal;
        for (auto &ch : local_protocal) ch = std::tolower(ch);
        if (local_protocal == "http") return default_http_flags();
        if (local_protocal == "https") return default_https_flags();
        return 0;
    }

    std::string format_message(DWORD dw)
    {
        LPVOID lpMsgBuf;
        FormatMessageA (
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );
        std::string msg = reinterpret_cast<char *>(lpMsgBuf);
        LocalFree(lpMsgBuf);
        return msg;
    }

    std::string http_request_core(const URL &url, http::VERB verb, const std::string &data) {
        using namespace orz;
        std::string report;

        // Open
        HINTERNET hInternet = InternetOpenA("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if(hInternet == NULL) {
            ORZ_LOG(INFO) << "Internet open error: " << std::hex << format_message(GetLastError());
            return report;
        }
        need close_internet(InternetCloseHandle, hInternet);

        // Connect
        HINTERNET hConnect = InternetConnectA(hInternet,
                                              url.host().c_str(),
                                              defulat_protocol_port(url.protocol(), url.port()),
                                              NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if(hConnect == NULL) {
            ORZ_LOG(INFO) << "Internet connect error: " << std::hex << format_message(GetLastError());
            return report;
        }
        need close_connect(InternetCloseHandle, hConnect);

        // Request
        HINTERNET hOpenRequest = HttpOpenRequestA(hConnect, verb_string(verb), url.target().c_str(), NULL, NULL,
                                                  (LPCSTR*)"*/*", defulat_protocol_flags(url.protocol()), 0);
        if(hOpenRequest == NULL) {
            ORZ_LOG(INFO) << "Http open request error: " << std::hex << format_message(GetLastError());
            return report;
        }
        need close_request(InternetCloseHandle, hOpenRequest);

        std::cout << data << std::endl;

        std::string header = "Content-Type:application/x-www-form-urlencoded; charset=utf-8";
        // std::string header = "Content-Type:application/json; charset=utf-8";

        // Send
        BOOL bRequest = HttpSendRequestA(hOpenRequest,
                                         const_cast<char *>(header.data()), header.size(),
                                         const_cast<char *>(data.data()), data.size());

        if (!bRequest) {
            ORZ_LOG(INFO) << "Http send request error: " << std::hex << format_message(GetLastError());
            return report;
        }

        binary buffer;
        char szBuffer[1024] = {0};
        DWORD dwByteRead = 0;
        while (InternetReadFile(hOpenRequest, szBuffer, sizeof(szBuffer), &dwByteRead) && dwByteRead > 0)
        {
            buffer.write(szBuffer, dwByteRead);
        }

        report = std::string(buffer.data<char>(), buffer.size());

        return report;
    }
}

#endif //ORZ_NET_HTTP_WIN_H
