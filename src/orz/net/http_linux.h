//
// Created by xif on 18-1-18.
//

#ifndef ORZ_NET_HTTP_LINUX_H
#define ORZ_NET_HTTP_LINUX_H

#include "orz/utils/log.h"
#include "orz/io/jug/binary.h"
#include "orz/mem/need.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cstring>

#ifndef WITH_OPENSSL

#else   // WITH_OPENSSL

#include <cstdio>
#include <openssl/ssl.h>
#include <openssl/err.h>

#endif  // !WITH_OPENSSL

namespace orz {

    class stream {
    public:
        stream(int fd, bool with_ssl = false)
                : m_fd(fd), m_with_ssl(with_ssl) {
            if (with_ssl) {
#ifndef WITH_OPENSSL
                ORZ_LOG(ERROR) << "Can not open a stream without OpenSSL." << crash;
#endif   // !WITH_OPENSSL
            } else {
                return;
            }

            SSL_load_error_strings();
            SSLeay_add_ssl_algorithms();

            m_ssl_ctx = SSL_CTX_new(SSLv23_client_method());
            if (m_ssl_ctx == nullptr) {
                ERR_print_errors_fp(stdout);
                ORZ_LOG(ERROR) << "Can not new SSL_CTX." << crash;
            }
            m_ssl = SSL_new(m_ssl_ctx);
            if (m_ssl == nullptr) {
                SSL_CTX_free(m_ssl_ctx);
                ERR_print_errors_fp(stdout);
                ORZ_LOG(ERROR) << "Can not new SSL." << crash;
            }
            SSL_set_fd(m_ssl, m_fd);
            if (SSL_connect(m_ssl) < 0) {
                SSL_free(m_ssl);
                SSL_CTX_free(m_ssl_ctx);
                ORZ_LOG(ERROR) << "Can not connect SSL to " << fd << crash;
            }
            // verify
        }

        bool const verify() {
            if (!m_with_ssl) return true;
#ifndef WITH_OPENSSL
            ORZ_LOG(ERROR) << "Can not verify a stream without OpenSSL." << crash;
#endif   // !WITH_OPENSSL
            auto cipher = SSL_get_cipher(m_ssl);
            auto cert = SSL_get_certificate(m_ssl);
            if (cert == nullptr) return false;
            need free_cert(X509_free, cert);
            auto subject = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
            auto issuer = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
            // TODO: really verify cert
            return true;
        }

        ssize_t read(void *data, size_t length) {
            if (m_with_ssl) {
#ifndef WITH_OPENSSL
                ORZ_LOG(ERROR) << "Can not read a stream without OpenSSL." << crash;
#endif   // !WITH_OPENSSL
                return SSL_read(m_ssl, data, length);
            }
            return ::read(m_fd, data, length);
        }

        ssize_t write(const void *data, size_t length) {
            if (m_with_ssl) {
#ifndef WITH_OPENSSL
                ORZ_LOG(ERROR) << "Can not write a stream without OpenSSL." << crash;
#endif   // !WITH_OPENSSL
                return SSL_write(m_ssl, data, length);
            }
            return ::write(m_fd, data, length);
        }

        ssize_t readline(void *data, size_t length) {
            ssize_t offset = 0;
            while (true) {
                ssize_t read_size = read(data + offset, 1);
                if (read_size <= 0) return read_size;
                ++offset;
                if (offset >= length) return length;
                if (reinterpret_cast<char *>(data)[offset - 1] == '\n') return offset;
            }
            return -1;
        }

        ~stream() {
#ifdef WITH_OPENSSL
            if (m_with_ssl) {
                SSL_shutdown(m_ssl);
                SSL_free(m_ssl);
                SSL_CTX_free(m_ssl_ctx);
            }
#endif
        }
    private:
        stream(const stream &other) = delete;
        const stream &operator=(const stream &other) = delete;

        int m_fd = 0;
        bool m_with_ssl = false;
#ifdef WITH_OPENSSL
        SSL_CTX *m_ssl_ctx = nullptr;
        SSL *m_ssl = nullptr;
#endif

    };

    int defulat_protocol_port(const std::string &protocal, int port) {
        if (port > 0) return port;
        std::string local_protocal = protocal;
        for (auto &ch : local_protocal) ch = std::tolower(ch);
        if (local_protocal == "http") return 80;
        if (local_protocal == "https") return 443;
        return 0;
    }

    bool need_ssl(const std::string &protocal) {
        std::string local_protocal = protocal;
        for (auto &ch : local_protocal) ch = std::tolower(ch);
        if (local_protocal == "https") return true;
        return false;
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
        std::string content_type = "Content-Type: application/x-www-form-urlencoded\r\n";
        std::string content_length = Concat("Content-Length: ", data.size(), "\r\n\r\n");
        std::string content = Concat(content_page, content_host, content_agent, content_type, content_length);
        content.insert(content.end(), data.begin(), data.end());

        ORZ_LOG(INFO) << content;

        int sockfd = socket(host->h_addrtype, SOCK_STREAM, 0);
        if (sockfd < 0) ORZ_LOG(ERROR) << "socket error: " << url.host() << crash;
        need close_sockfd(close, sockfd);


        struct sockaddr_in servaddr;
        std::memset(&servaddr, 0, sizeof(servaddr));

        servaddr.sin_family = host->h_addrtype;
        servaddr.sin_port = htons(port);
        servaddr.sin_addr = host_addr;

        if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
            ORZ_LOG(ERROR) << "connect error: " << url.host() << crash;

        stream socket_stream(sockfd, need_ssl(url.protocol()));

        socket_stream.write(content.data(), content.size());

        binary buffer;
        char szBuffer[227] = {0};
        int dwByteRead;

        while ((dwByteRead = socket_stream.readline(szBuffer, sizeof(szBuffer))) > 0) {
            // ORZ_LOG(INFO) << "Reading...";
            szBuffer[dwByteRead] = '\0';
            std::string tmp(szBuffer, strlen(szBuffer));
            if (tmp.substr(0, strlen("Content-Length")) == "Content-Length")
                ORZ_LOG(INFO) << tmp;
            if (tmp.substr(0, strlen("Transfer-Encoding")) == "Transfer-Encoding")
            ORZ_LOG(INFO) << tmp;
            buffer.write(szBuffer, dwByteRead);


            std::cout << szBuffer;
            if (dwByteRead > 200) break;
        }
        ORZ_LOG(INFO) << "Read done.";

        report = std::string(buffer.data<char>(), buffer.size());

        return report;
    }
}

#endif //ORZ_NET_HTTP_LINUX_H
