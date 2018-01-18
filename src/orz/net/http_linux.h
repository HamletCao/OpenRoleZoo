//
// Created by xif on 18-1-18.
//

#ifndef ORZ_NET_HTTP_LINUX_H
#define ORZ_NET_HTTP_LINUX_H

#include "orz/utils/log.h"
#include "orz/io/jug/binary.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cstring>

namespace orz {

    int defulat_protocol_port(const std::string &protocal, int port) {
        if (port > 0) return port;
        std::string local_protocal = protocal;
        for (auto &ch : local_protocal) ch = std::tolower(ch);
        if (local_protocal == "http") return 80;
        if (local_protocal == "https") return 443;
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

    struct hostent *gethostbyname_local(const char *name, struct hostent &result_bufd) {
        struct hostent result_buf;
        struct hostent *result = nullptr;
        char buf[1024];
        size_t buflen = sizeof(buf);
        int err;
        if (gethostbyname_r(name, &result_buf, buf, buflen, &result, &err) != 0 || result == nullptr) {
            return nullptr;
        }
        ORZ_LOG(INFO) << &result_buf << " " << result;
        return result;
    }

    int readline_local(int fd, void *buff, size_t length) {
        int offset = 0;
        while (read(fd, buff + offset, 1) > 0) {
            ++offset;
            if (offset >= length) return length;
            if (reinterpret_cast<char *>(buff)[offset - 1] == '\n') return offset;
        }
        return 0;
    }

    std::string http_request_core(const URL &url, http::VERB verb, const std::string &data) {
        std::string report;

        struct hostent host_buf;
        struct hostent *host = gethostbyname_local(url.host().c_str(), host_buf);
        in_addr host_addr = *reinterpret_cast<in_addr *>(host->h_addr);
        std::string ip = inet_ntoa(host_addr);


        std::string content_page = Concat(verb_string(verb), " ", url.target(), " HTTP/1.1\r\n");
        int port = defulat_protocol_port(url.protocol(), url.port());
        std::string content_host = Concat("HOST: ", ip, "\r\n");
        std::string content_agent = Concat("User-Agent: ", "Microsoft Internet Explorer", "\r\n");
        std::string content_connection = Concat("Connection: ", "keep-alive", "\r\n");
        std::string content_alive = Concat("Keep-Alive: ", "timeout=10", "\r\n");
        std::string content_type = "Content-Type: application/x-www-form-urlencoded\r\n";
        std::string content_length = Concat("Content-Length: ", data.size(), "\r\n\r\n");
        std::string content = Concat(content_page, content_host, content_agent, content_type, content_length);
        content.insert(content.end(), data.begin(), data.end());

        ORZ_LOG(INFO) << content;

        int sockfd = socket(host->h_addrtype, SOCK_STREAM, 0);

        if (sockfd < 0) ORZ_LOG(ERROR) << "socket error: " << url.host() << crash;

        struct sockaddr_in servaddr;
        std::memset(&servaddr, 0, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        servaddr.sin_addr = host_addr;

        if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
            ORZ_LOG(ERROR) << "connect error: " << url.host() << crash;

        write(sockfd, content.data(), content.size());

        binary buffer;
        char szBuffer[1024] = {0};
        int dwByteRead;

        while ((dwByteRead = readline_local(sockfd, szBuffer, sizeof(szBuffer))) > 0) {
            // ORZ_LOG(INFO) << "Reading...";
            szBuffer[dwByteRead] = '\0';
            std::string tmp(szBuffer, strlen(szBuffer));
            if (tmp.substr(0, strlen("Content-Length")) == "Content-Length")
                ORZ_LOG(INFO) << tmp;
            if (tmp.substr(0, strlen("Transfer-Encoding")) == "Transfer-Encoding")
            ORZ_LOG(INFO) << tmp;
            buffer.write(szBuffer, dwByteRead);
        }
        ORZ_LOG(INFO) << "Read done.";

        report = std::string(buffer.data<char>(), buffer.size());

        return report;
    }
}

#endif //ORZ_NET_HTTP_LINUX_H
