//
// Created by lby on 2018/1/15.
//

#ifndef ORZ_IO_I_H
#define ORZ_IO_I_H

#include "jug/binary.h"

#include <istream>

namespace orz {
    binary read_file(const std::string &filename);

    std::string read_txt_file(const std::string &filename);

    class imemorystream : public std::istream {
    public:
        imemorystream(const void *data, size_t size);

    private:
        class imemorybuffer : public std::streambuf {

        public:
            imemorybuffer(const void *data, size_t size);

        protected:
            int_type overflow(int_type c) override;

            std::streambuf *setbuf(char *s, std::streamsize n);

            int_type underflow() override;

            int_type uflow() override;

            streampos seekoff(streamoff off, ios_base::seekdir way,
                              ios_base::openmode which) override;

            streampos seekpos(streampos sp, ios_base::openmode which) override;

        private:
            const void *m_data;
            size_t m_size;
        };

        imemorybuffer m_buffer;
    };
}

#endif //ORZ_IO_I_H
