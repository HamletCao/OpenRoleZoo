//
// Created by lby on 2018/1/15.
//

#include "orz/io/i.h"
#include <fstream>
#include <sstream>

namespace orz {
    binary read_file(const std::string &filename) {
        binary bin;
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return bin;
        in.seekg(0, std::ios::end);
        auto size = in.tellg();
        bin.resize(size_t(size));
        in.seekg(0, std::ios::beg);
        in.read(bin.data<char>(), bin.size());
        in.close();
        return bin;
    }

	std::string read_txt_file(const std::string& filename)
	{
		std::ifstream in(filename);
		std::ostringstream tmp;
		tmp << in.rdbuf();
		return tmp.str();
	}

    imemorystream::imemorystream(const void *data, size_t size) :
            std::istream(&m_buffer),
            m_buffer(data, size) {
    }

    imemorystream::imemorybuffer::imemorybuffer(const void *data, size_t size) :
            m_data(data), m_size(size) {
        setbuf((char *) data, size);
    }

    int imemorystream::imemorybuffer::overflow(int c) {
        return c;
    }

    std::streambuf *imemorystream::imemorybuffer::setbuf(char *s, std::streamsize n) {
        // setp(s, s + n);  // do not set output pointer
        setg(s, s, s + n);
        return this;
    }

    int imemorystream::imemorybuffer::underflow() {
        return EOF;
    }

    int imemorystream::imemorybuffer::uflow() {
        return EOF;
    }

    std::streampos
    imemorystream::imemorybuffer::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) {
        if (!(which | ios_base::in)) return gptr() - eback();
        // TODO: check which for omemorystream
        auto pos = gptr();
        switch (way) {
            case ios_base::beg:
                pos = eback() + off;
                break;
            case ios_base::cur:
                pos = gptr() + off;
                break;
            case ios_base::end:
                pos = egptr() + off;
                break;
            default:
                pos = gptr();
                break;
        }
        setg(eback(), pos, egptr());
        return gptr() - eback();
    }

    std::streampos imemorystream::imemorybuffer::seekpos(std::streampos sp, std::ios_base::openmode which) {
        if (!(which | ios_base::in)) return gptr() - eback();
        // TODO: check which for omemorystream
        auto pos = eback() + sp;
        setg(eback(), pos, egptr());
        return sp;
    }
}
