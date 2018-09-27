//
// Created by kier on 2018/9/25.
//

#include "orz/tools/resources.h"

#include "orz/io/dir.h"
#include "orz/io/walker.h"
#include "orz/utils/log.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <cstdio>
#include <memory>

#pragma warning(disable: 4996)

namespace orz {
    namespace resources {
        static const char *const code_header[] = {
                "#ifndef _INC_ORZ_RESOURCES_AUTO_COMPILATION_H",
                "#define _INC_ORZ_RESOURCES_AUTO_COMPILATION_H",
                "",
                "#ifdef __cplusplus",
                "extern \"C\" {",
                "#endif",
                "",
                "#include <stddef.h>",
                "",
                "struct orz_resources {",
                "    const char *data;",
                "    size_t size;",
                "};",
                "",
                "const struct orz_resources orz_resources_get(const char *key);",
                // "void orz_resources_list();",
                "",
                "#ifdef __cplusplus",
                "}",
                "#endif",
                "",
                "#endif //_INC_ORZ_RESOURCES_AUTO_COMPILATION_H",
                "",
        };

        static const char *const code_source_include[] = {
                "#include <string.h>",
                "#include <stdio.h>",
        };

        static const char *const code_source_declare_ELFhash[] = {
                "static unsigned int ELFhash(const char *str) {",
                "    unsigned int hash = 0;",
                "    unsigned int x = 0;",
                "    while (*str) {",
                "        hash = (hash << 4) + *str;",
                "        if ((x = hash & 0xf0000000) != 0) {",
                "            hash ^= (x >> 24);",
                "            hash &= ~x;",
                "        }",
                "        str++;",
                "    }",
                "    return (hash & 0x7fffffff);",
                "}",
        };

        static const char *const code_source_declare_orz_resources_node[] = {
                "struct orz_resources_node {",
                "    const char *key;",
                "    unsigned int hash;",
                "    int next;",
                "",
                "    const char *data;",
                "    size_t size;",
                "};",
        };

        static const char *const code_source_declare_orz_resources_table_head[] = {
                "struct orz_resources_node orz_resources_table[] = {",
        };

        static const char *const code_source_declare_orz_resources_table_tail[] = {
                "};",
        };

        static const char *const code_source_declare_orz_resources_table_size[] = {
                "static const unsigned int orz_resources_table_size =",
                "    sizeof(orz_resources_table) / sizeof(orz_resources_table[0]);",
        };

        static const char *const code_source_declare_orz_resources_table_find[] = {
                "struct orz_resources_node *orz_resources_table_find(const char *key)",
                "{",
                "    if (orz_resources_table_size == 0) return NULL;",
                "    unsigned int hash = ELFhash(key);",
                "    unsigned int index = hash % orz_resources_table_size;",
                "    while (orz_resources_table[index].key) {",
                "        struct orz_resources_node *node = &orz_resources_table[index];",
                "        if (hash == node->hash && strcmp(key, node->key) == 0) {",
                "            return node;",
                "        }",
                "        {",
                "            int next_index = node->next;",
                "            if (next_index < 0) break;",
                "            index = (unsigned int)(next_index);",
                "        }",
                "    }",
                "    return NULL;",
                "}",
        };

        static const char *const code_source_declare_orz_resources_get[] = {
                "const struct orz_resources orz_resources_get(const char *key)",
                "{",
                "    struct orz_resources resources;",
                "    struct orz_resources_node *node;",
                "",
                "    resources.data = NULL;",
                "    resources.size = 0;",
                "    if (!key)",
                "    {",
                "        return resources;",
                "    }",
                "    if (key[0] == '@')",
                "    {",
                "        key++;",
                "    }",
                "    node = orz_resources_table_find(key);",
                "    if (!node)",
                "    {",
                "        return resources;",
                "    }",
                "    resources.data = node->data;",
                "    resources.size = node->size;",
                "    return resources;",
                "}",
        };

        static inline std::ostream &write_lines(std::ostream &out, const char *const *lines, size_t num) {
            for (size_t i = 0; i < num; ++i) {
                out << lines[i] << std::endl;
            }
            return out;
        }

        template<size_t _Size>
        static inline std::ostream &write_lines(std::ostream &out, const char *const (&lines)[_Size]) {
            return write_lines(out, lines, _Size);
        }

        static inline unsigned int ELFhash(const char *str) {
            unsigned int hash = 0;
            unsigned int x = 0;
            while (*str) {
                hash = (hash << 4) + *str;
                if ((x = hash & 0xf0000000) != 0) {
                    hash ^= (x >> 24);
                    hash &= ~x;
                }
                str++;
            }
            return (hash & 0x7fffffff);
        }

        class resources_hash_node {
        public:
            using hash_type = unsigned int;

            resources_hash_node() = default;

            explicit resources_hash_node(const std::string &key, const resources &value)
                    : key(key), hash(ELFhash(key.c_str())), value(value) {}

            std::string key;
            hash_type hash;
            int64_t next = -1;

            resources value;
        };

        class resources_hash_table {
        public:
            using self = resources_hash_table;

            using hash_type = unsigned int;
            using index_type = hash_type;

            explicit resources_hash_table(unsigned int size) : m_nodes(size_t(size > 2 ? size : 2), nullptr) {}

            resources_hash_table() : resources_hash_table(2) {}

            ~resources_hash_table() {
                for (auto node : m_nodes) {
                    delete node;
                }
            }

            resources_hash_table(const self &) = delete;

            self &operator==(const self &) = delete;

            resources_hash_node *insert(const std::string &key, const resources &value) {
                auto *found_node = this->find(key);
                if (found_node) {
                    found_node->value = value;
                    return found_node;
                }
                auto *new_node = new resources_hash_node(key, value);
                insert(new_node);
                return new_node;
            }

            // TODO: add erase method
            resources_hash_node *find(const std::string &key) {
                auto hash = ELFhash(key.c_str());
                auto index = index_type(hash % m_nodes.size());
                while (m_nodes[index] != nullptr) {
                    auto node = m_nodes[index];
                    if (hash == node->hash && std::strcmp(key.c_str(), node->key.c_str()) == 0) {
                        return m_nodes[index];
                    }
                    auto next_index = m_nodes[index]->next;
                    if (next_index < 0) break;
                    index = index_type(next_index);
                }
                return nullptr;
            }

            size_t size() const {
                return m_size;
            }

            const std::vector<resources_hash_node *> &nodes() const {
                return m_nodes;
            }

        private:
            void insert(resources_hash_node *node) {
                if (node == nullptr) return;

                if (m_size >= m_nodes.size() / 2) {
                    rehash(m_nodes.size() * 2);
                }

                ++m_size;
                auto hash = node->hash;
                auto index = index_type(hash % m_nodes.size());
                auto first_aid_index = index;
                if (m_nodes[index] == nullptr) {
                    m_nodes[index] = node;
                    return;
                }
                do {
                    auto anchor = m_nodes[index];
                    auto anchor_hash = anchor->hash;
                    auto anchor_index = index_type(anchor_hash % m_nodes.size());
                    // if (anchor_hash == hash && anchor->key == key) {}
                    if (anchor_index == first_aid_index) {
                        auto next_index = anchor->next;
                        if (next_index < 0) {
                            // a.1 insert at empty slot
                            node->next = -1;
                            anchor->next = insert_at_next_ready(node);
                            break;  // break when m_nodes[index]->next < 0
                        }
                        index = index_type(next_index);
                    } else {
                        // b.1 find pre-next value
                        auto pre_next_index = index_type(anchor_hash % m_nodes.size());
                        if (m_nodes[pre_next_index] == nullptr) {
                            m_nodes[pre_next_index] = anchor;
                            m_nodes[index] = node;
                            break;
                        }
                        while (m_nodes[pre_next_index]->next >= 0 &&
                               m_nodes[pre_next_index]->next != index) {
                            pre_next_index = index_type(m_nodes[pre_next_index]->next);
                        }
                        m_nodes[pre_next_index]->next = update_conflict_node(anchor);
                        m_nodes[index] = node;
                        break;  // break when hash conflict
                    }
                } while (m_nodes[index] != nullptr);    ///< always true
            }

            index_type update_conflict_node(resources_hash_node *node) {
                auto hash = node->hash;
                auto index = index_type(hash % m_nodes.size());
                if (m_nodes[index] == nullptr) {
                    m_nodes[index] = node;
                    return index;
                }
                return insert_at_next_ready(node);
            }

            index_type insert_at_next_ready(resources_hash_node *node) {
                while (m_nodes[m_next_ready] != nullptr) m_next_ready = (m_next_ready + 1) % m_nodes.size();
                auto index = index_type(m_next_ready);
                m_next_ready = (m_next_ready + 1) % m_nodes.size();
                m_nodes[index] = node;
                return index;
            }

            void rehash(size_t size) {
                if (size < m_nodes.size()) size = m_nodes.size();
                resources_hash_table temp((unsigned int) (size));
                for (auto &node : m_nodes) {
                    if (node == nullptr) continue;
                    node->next = -1;
                    temp.insert(node);
                }
                m_nodes = temp.m_nodes;
                m_next_ready = temp.m_next_ready;
                m_size = temp.m_size;
                temp.m_nodes.clear();
                temp.m_next_ready = 0;
                temp.m_size = 0;
            }

        private:
            std::vector<resources_hash_node *> m_nodes;
            size_t m_next_ready = 0;
            size_t m_size = 0;
        };

        class code_block {
        public:
            using self = code_block;

            static const size_t BUFFER_SIZE = 1024 * 1024;

            code_block()
                    : m_buffer_size(BUFFER_SIZE), m_buffer(new char[BUFFER_SIZE], std::default_delete<char[]>()) {}

            static inline int write_byte(std::ostream &out, int byte) {
                byte &= 0xff;
                switch (byte)
                {
                    case '\0': out << R"(\0)"; return 2;
                    case '\'': out << R"(\')"; return 2;
                    case '\"': out << R"(\")"; return 2;
                    case '\?': out << R"(\?)"; return 2;
                    case '\\': out << R"(\\)"; return 2;
                    case '\a': out << R"(\a)"; return 2;
                    case '\b': out << R"(\b)"; return 2;
                    case '\f': out << R"(\f)"; return 2;
                    case '\n': out << R"(\n)"; return 2;
                    case '\r': out << R"(\r)"; return 2;
                    case '\t': out << R"(\t)"; return 2;
                    case '\v': out << R"(\v)"; return 2;
                    default: {
                        if (byte >= 0x20 && byte <= 0x7E) {
                            out << char(byte); return 1;
                        } else {
                            char temp[5];
                            std::sprintf(temp, "\\x%02x", byte);
                            out << temp; return 4;
                        }
                    }
                }
            }

            static inline int write_byte_hex(std::ostream &out, int byte) {
                byte &= 0xff;
                char temp[5];
                std::sprintf(temp, "\\x%02x", byte);
                out << temp;
                return 4;
            }

            std::ostream &data(std::ostream &out, std::istream &mem,
                               const std::string &indent = "",
                               size_t *size = nullptr) {
                static const int loop_size = 96;
                int write_number = 0;
                size_t write_size = 0;
                std::ostringstream out_buffer;

                char *buffer = m_buffer.get();
                const size_t bufer_size = m_buffer_size;

                out << std::hex;
                out << "\"";
                while (mem.good()) {
                    mem.read(buffer, bufer_size);
                    auto read_size = mem.gcount();
                    for (std::streamsize i = 0; i < read_size; ++i) {
                        auto byte = buffer[i];
                        // out << "\\x" << std::setw(2) << std::setfill('0') << ((unsigned int)(byte) & 0xff);
                        write_number += write_byte_hex(out_buffer, byte);
                        ++write_size;
                        if (write_number >= loop_size) {
                            out_buffer << "\"" << std::endl << "\"";
                            out << out_buffer.str();
                            out_buffer.str("");
                            write_number = 0;
                        }
                    }
                }
                if (write_number > 0) {
                    out_buffer << "\"" << std::endl << "\"";
                    out << out_buffer.str();
                    out_buffer.str("");
                }
                out << indent << "\"";
                if (size) *size = write_size;
                return out;
            }

        private:
            size_t m_buffer_size = 0;
            std::shared_ptr<char> m_buffer;
        };

        std::string trim(const std::string &line) {
            std::string pattern = " \r\n\t";
            auto left = line.find_first_not_of(pattern);
            if (left == std::string::npos) return "";
            auto right = line.find_last_not_of(pattern);
            if (right == std::string::npos) return "";
            if (right >= left) {
                return line.substr(left, right - left + 1);
            }
            return "";
        }

        static inline std::string get_filename(const std::string &path) {
            auto win_sep_pos = path.rfind('\\');
            auto unix_sep_pos = path.rfind('/');
            auto sep_pos = win_sep_pos;
            if (sep_pos == std::string::npos) sep_pos = unix_sep_pos;
            else if (unix_sep_pos != std::string::npos && unix_sep_pos > sep_pos) sep_pos = unix_sep_pos;
            if (sep_pos == std::string::npos) {
                return path;
                return std::string();
            }
            return path.substr(sep_pos + 1);
        }

        bool
        compiler::compile(const std::vector<orz::resources::resources> &in_resources, std::ostream &out_header,
                          std::ostream &out_source, const std::string &val_header_path) {
            // build table
            resources_hash_table table;
            for (auto &resources : in_resources) {
                resources_hash_node *try_found = table.find(resources.url);
                if (try_found) {
                    auto &conflict_resources = try_found->value;
                    std::ostringstream oss;
                    oss << "[Error] line(" << resources.line << "): "
                        << "Conflict URL with line(" << conflict_resources.line << ") \"" << resources.url << "\"";
                    m_last_error_message = oss.str();
                    return false;
                }
                table.insert(resources.url, resources);
            }
            const auto &nodes = table.nodes();
            std::vector<std::shared_ptr<std::ifstream>> in_files(nodes.size());
            // 1.0 open sources
            for (size_t i = 0; i < nodes.size(); ++i) {
                auto node = nodes[i];
                if (node == nullptr) continue;
                auto &res = node->value;
                auto &file = in_files[i];
                file = std::make_shared<std::ifstream>(res.path, std::ios::binary);
                if (!file->is_open()) {
                    std::ostringstream oss;
                    oss << "[Error] line(" << res.line << "): " << "Can not access file \"" << res.path << "\"";
                    m_last_error_message = oss.str();
                    return false;
                }
            }
            out_source << "#include \"" << val_header_path << "\"" << std::endl;
            out_source << std::endl;

            write_lines(out_source, code_source_include) << std::endl;
            write_lines(out_source, code_source_declare_ELFhash) << std::endl;
            write_lines(out_source, code_source_declare_orz_resources_node) << std::endl;
            write_lines(out_source, code_source_declare_orz_resources_table_head);

            code_block coder;
            // 1.2 write table
            for (size_t i = 0; i < nodes.size(); ++i) {
                auto node = nodes[i];
                if (node == nullptr) {
                    out_source << "{NULL, 0, -1, NULL, 0}," << std::endl;
                    continue;
                }
                auto &res = node->value;
                auto &file = *in_files[i];

                out_source << std::dec;
                out_source << "{ \"" << res.url << "\", " << node->hash << ", " << node->next << "," << std::endl;

                size_t size = 0;
                coder.data(out_source, file, std::string(""), &size) << ", ";

                file.close();

                out_source << std::dec;
                out_source << size << "UL, ";
                out_source << "}," << std::endl;
            }

            write_lines(out_source, code_source_declare_orz_resources_table_tail) << std::endl;
            write_lines(out_source, code_source_declare_orz_resources_table_size) << std::endl;
            write_lines(out_source, code_source_declare_orz_resources_table_find) << std::endl;
            write_lines(out_source, code_source_declare_orz_resources_get) << std::endl;

            // 2.0 write header
            write_lines(out_header, code_header);
            return true;
        }

        bool compiler::compile(std::istream &in_source, std::ostream &out_header, std::ostream &out_source,
                               const std::string &val_header_path) {
            size_t line_number = 0;
            std::string line;
            std::vector<resources> list;
            while (std::getline(in_source, line)) {
                line_number++;

                line = trim(line);
                if (line.empty()) continue;
                if (line[0] == annotation) continue;

                auto split_index = line.find(':');

                if (split_index == std::string::npos) {
                    std::ostringstream oss;
                    oss << "[Error] line(" << line_number << "): " << "Syntax error.";
                    m_last_error_message = oss.str();
                    return false;
                }

                resources res;
                res.line = line_number;
                res.url = trim(line.substr(0, split_index));
                res.path = trim(line.substr(split_index + 1));

                list.push_back(res);
            }

            return compile(list, out_header, out_source, val_header_path);
        }

        bool compiler::compile(std::istream &in_source, const std::string &header_filename,
                               const std::string &source_filename) {
            std::ofstream out_header(header_filename);
            if (!out_header.is_open()) {
                std::ostringstream oss;
                oss << "[Error] : " << "Can not open output file \"" << header_filename << "\"";
                m_last_error_message = oss.str();
                return false;
            }
            std::ofstream out_source(source_filename);
            if (!out_source.is_open()) {
                std::ostringstream oss;
                oss << "[Error] : " << "Can not open output file \"" << source_filename << "\"";
                m_last_error_message = oss.str();
                return false;
            }
            return compile(in_source, out_header, out_source, get_filename(header_filename));
        }

        bool compiler::compile(const std::string &path, const std::string &header_filename,
                               const std::string &source_filename) {
            if (orz::isfile(path)) {
                std::ifstream in_source(path);
                if (!in_source.is_open()) {
                    std::ostringstream oss;
                    oss << "[Error] : " << "Can not access input file \"" << path << "\"";
                    m_last_error_message = oss.str();
                    return false;
                }
                return compile(in_source, header_filename, source_filename);
            } else if (orz::isdir(path)) {
                auto filenames = orz::FindFilesRecursively(path);
                Log(INFO) << "Find " << filenames.size() << " files in \"" << path << "\"";
                std::vector<resources> in_resources(filenames.size());
                for (size_t i = 0; i < filenames.size(); ++i) {
                    auto &filename = filenames[i];
                    resources res;
                    res.line = i + 1;
                    res.url = std::string("/") + filename;
                    res.path = orz::Join({path, filename}, orz::FileSeparator());
                    for (auto &ch : res.url) {
                        if (ch == '\\') ch = '/';
                    }
                    in_resources[i] = res;
                }
                std::ofstream out_header(header_filename);
                if (!out_header.is_open()) {
                    std::ostringstream oss;
                    oss << "[Error] : " << "Can not open output file \"" << header_filename << "\".";
                    m_last_error_message = oss.str();
                    return false;
                }
                std::ofstream out_source(source_filename);
                if (!out_source.is_open()) {
                    std::ostringstream oss;
                    oss << "[Error] : " << "Can not open output file \"" << source_filename << "\".";
                    m_last_error_message = oss.str();
                    return false;
                }
                return compile(in_resources, out_header, out_source, get_filename(header_filename));
            } else {
                std::ostringstream oss;
                oss << "[Error] : " << "Can not access input path \"" << path << "\", is a file or dir?";
                m_last_error_message = oss.str();
                return false;
            }
            return false;
        }
    }
}
