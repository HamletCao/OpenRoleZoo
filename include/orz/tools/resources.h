//
// Created by kier on 2018/9/25.
//

#ifndef ORZ_TOOLS_RESOURCES_H
#define ORZ_TOOLS_RESOURCES_H

#include <string>
#include <istream>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>

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
            explicit resources_hash_node(const std::string &key, ssize_t next = -1)
                    : key(key), hash(ELFhash(key.c_str())), next(next) {}

            std::string key;
            hash_type hash;
            ssize_t next = -1;
        };

        class resources_has_table {
        public:
            using self = resources_has_table;

            using hash_type = unsigned int;
            using index_type = hash_type;

            explicit resources_has_table(unsigned int size) : m_nodes(size_t(size > 2 ? size : 2), nullptr) {}

            resources_has_table() : resources_has_table(2) {}

            ~resources_has_table() {
                for (auto node : m_nodes) {
                    delete node;
                }
            }

            resources_has_table(const self &) = delete;
            self &operator==(const self &) = delete;

            resources_hash_node *insert(const std::string &key) {
                auto *found_node = this->find(key);
                if (found_node) {
                    // TODO: set value
                    return found_node;
                }
                auto *new_node = new resources_hash_node(key);
                insert(new_node);
                return new_node;
            }

            // TODO: add erase method
            resources_hash_node *find(const std::string &key) {
                auto hash = ELFhash(key.c_str());
                auto index = index_type(hash % m_nodes.size());
                while (m_nodes[index] != nullptr)
                {
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
                                m_nodes[pre_next_index]->next != index)
                        {
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
                resources_has_table temp((unsigned int)(size));
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
            std::vector<resources_hash_node*> m_nodes;
            size_t m_next_ready = 0;
            size_t m_size = 0;
        };

        class code_block {
        public:
            static bool is_number(char ch) {
                return '0' <= ch && ch <= '9';
            }

            static bool is_letter(char ch) {
                return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
            }

            static bool is_underline(char ch) {
                return ch == '_';
            }

            static std::string to_variable(const std::string &url) {
                auto url_copy = url;
                for (auto &ch : url_copy) {
                    if (is_number(ch) || is_letter(ch) || is_underline(ch)) {
                        continue;
                    }
                    ch = '_';
                }
                return std::string("orz_resources") + url_copy;
            }

            /**
             * static char <to_variable(url)>[] =
             * "\xAB\xCD"
             * "";
             */
            static void declare(std::ostream &out, const std::string &url, std::istream &mem,
                    const std::string &indent = "") {
                static const int loop_size = 19;
                out << indent << "static char " << to_variable(url) << "[] =" << std::endl;
                int write_number = 0;
                bool in_double_quotes = false;
                char byte;
                out << std::hex;
                while (mem.read(&byte, 1)) {
                    if (!in_double_quotes) {
                        out << indent << "\"";
                        in_double_quotes = true;
                    }
                    out << "\\x" << std::setw(2) << std::setfill('0') << int(byte);
                    ++write_number;
                    if (write_number >= loop_size) {
                        out << "\"" << std::endl;
                        in_double_quotes = false;
                        write_number = 0;
                    }
                }
                if (in_double_quotes) {
                    out << "\"" << std::endl;
                }
                out << indent << "\"\";" << std::endl;
            }

            /**
             * if (strcmp(<var_url>, "<url>") == 0)
             * {
             *     <var_resources>.data = <to_variable(url)>;
             *     <var_resources>.size = sizeof(<to_variable(url)>) - 1;
             * }
             */
            static void usage(std::ostream &out, const std::string &url,
                    const std::string &var_url,
                    const std::string &var_resources,
                    const std::string &indent = "") {
                auto resources_data  = to_variable(url);
                out << indent << "if (strcmp(" << var_url << ", \"" << url <<"\") == 0)" << std::endl;
                out << indent << "{" << std::endl;
                out << indent << "    " << var_resources << ".data = " << resources_data << ";" << std::endl;
                out << indent << "    " << var_resources << ".size = sizeof(" << resources_data << ") - 1;" << std::endl;
                out << indent << "}" << std::endl;
            }
        };

        std::string trim(const std::string &line) {
            std::string pattern = " \t\r\n";
            auto left = line.find_first_not_of(pattern);
            if (left == std::string::npos) return "";
            auto right = line.find_last_not_of(pattern);
            if (right == std::string::npos) return "";
            if (right >= left) {
                return line.substr(left, right - left + 1);
            }
            return "";
        }

        struct resources
        {
        public:
            size_t line = 0;
            std::string url;
            std::string path;
        };

        class compiler {
        public:
            static const char annotation = '#';

            bool compile(const std::vector<resources> &in_resources, std::ostream &out_header, std::ostream &out_source,
                    const std::string &val_header_path = "orz_resources.h") {
                // TODO: check if resources urls conflict
                std::vector<std::ifstream> in_files(in_resources.size());
                // 1.0 open sources
                for (size_t i = 0; i < in_resources.size(); ++i) {
                    auto &res = in_resources[i];
                    auto &file = in_files[i];
                    file.open(res.path);
                    if (!file.is_open()) {
                        std::ostringstream oss;
                        oss << "[Error] line(" << res.line << "): " << "Can not access \"" << res.path << "\"";
                        m_last_error_message = oss.str();
                        return false;
                    }
                }
                out_source << "#include \"" << val_header_path << "\"" << std::endl;
                out_source << std::endl;
                out_source << "#include <string.h>" << std::endl;
                out_source << "#include <stdio.h>" << std::endl;
                out_source << std::endl;

                // 1.1 write declare
                for (size_t i = 0; i < in_files.size(); ++i) {
                    auto &res = in_resources[i];
                    auto &file = in_files[i];
                    code_block::declare(out_source, res.url, file);
                    out_source << std::endl;
                }

                out_source << "const struct orz_resources orz_resources_get(const char *key)" << std::endl;
                out_source << "{" << std::endl;
                out_source << "    struct orz_resources resources;" << std::endl;
                out_source << "    resources.data = NULL;" << std::endl;
                out_source << "    resources.size = 0;" << std::endl;
                out_source << "    if (!key)" << std::endl;
                out_source << "    {" << std::endl;
                out_source << "        return resources;" << std::endl;
                out_source << "    }" << std::endl;
                out_source << "    if (key[0] == '@')" << std::endl;
                out_source << "    {" << std::endl;
                out_source << "        key++;" << std::endl;
                out_source << "    }" << std::endl;

                // 1.2 write usage
                for (size_t i = 0; i < in_files.size(); ++i) {
                    auto &res = in_resources[i];
                    if (i) out_source << "    else" << std::endl;
                    code_block::usage(out_source, res.url, "key", "resources", "    ");
                    // out_source << std::endl;
                }

                out_source << "    return resources;" << std::endl;
                out_source << "}" << std::endl;

                // write header
                static const size_t header_line_number = sizeof(code_header) / sizeof(code_header[0]);
                for (size_t i = 0; i < header_line_number; ++i) {
                    out_header << code_header[i] << std::endl;
                }
                return true;
            }

            bool compile(std::istream &in_source, std::ostream &out_header, std::ostream &out_source) {
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

                return compile(list, out_header, out_source);
            }

        private:
            std::string m_last_error_message;
        };
    }
}


#endif //ORZ_TOOLS_RESOURCES_H
