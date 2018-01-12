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

    std::string http_request_core(const URL &url, http::VERB verb, const std::string &data) {
        using namespace orz;
        std::string report;

        // Open
        HINTERNET hInternet = InternetOpenA("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if(hInternet == NULL) {
            ORZ_LOG(INFO) << "Open internet error: " << std::hex << GetLastError();
        }
        need close_internet(InternetCloseHandle, hInternet);

        // Connect
        HINTERNET hConnect = InternetConnectA(hInternet,
                                              url.host().c_str(),
                                              defulat_protocol_port(url.protocol(), url.port()),
                                              NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        need close_connect(InternetCloseHandle, hConnect);

        // Request
        HINTERNET hOpenRequest = HttpOpenRequestA(hConnect, verb_string(verb), url.target().c_str(), NULL, NULL,
                                                  (LPCSTR*)"*/*", INTERNET_FLAG_DONT_CACHE, 1);
        need close_request(InternetCloseHandle, hOpenRequest);

        // Send
        BOOL bRequest = HttpSendRequestA(hOpenRequest, NULL, 0, NULL, 0);

        if (!bRequest) ORZ_LOG(orz::FATAL) << "Request unknown error" << crash;

        binary buffer;
        char szBuffer[1024] = {0};
        DWORD dwByteRead = 0;
        while (InternetReadFile(hOpenRequest, szBuffer, sizeof(szBuffer), &dwByteRead) && dwByteRead > 0)
        {
            buffer.write(szBuffer, dwByteRead);
        }

        ORZ_LOG(INFO) << "Read size: " << buffer.size();

        std::string str(buffer.data<char>(), buffer.size());

        ORZ_LOG(INFO) << "Report: " << str;

        return std::string();
    }
}

#endif //ORZ_NET_HTTP_WIN_H
