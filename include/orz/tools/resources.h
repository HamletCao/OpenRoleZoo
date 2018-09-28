//
// Created by kier on 2018/9/25.
//

#ifndef ORZ_TOOLS_RESOURCES_H
#define ORZ_TOOLS_RESOURCES_H

#include <string>
#include <vector>
#include <istream>
#include <ostream>

namespace orz {

    namespace resources {
        struct resources {
        public:
            resources() = default;
            explicit resources(const std::string &url, const std::string &path, size_t line = 0)
                    : line(line), url(url), path(path) {}

            size_t line = 0;
            std::string url;
            std::string path;
        };

        class compiler {
        public:
            static const char annotation = '#';

            bool compile(const std::vector<resources> &in_resources, std::ostream &out_header, std::ostream &out_source,
                         const std::string &val_header_path = "orz_resources.h");

            bool compile(std::istream &in_source, std::ostream &out_header, std::ostream &out_source,
                         const std::string &val_header_path = "orz_resources.h");

            bool compile(std::istream &in_source,
                         const std::string &header_filename,
                         const std::string &source_filename);

            bool compile(const std::string &path, const std::string &header_filename, const std::string &source_filename);

            bool up2date_header(const std::string &header_filename);

            const std::string &last_error_message() const {
                return m_last_error_message;
            }

        private:
            std::string m_last_error_message;
        };
    }
}


#endif //ORZ_TOOLS_RESOURCES_H
