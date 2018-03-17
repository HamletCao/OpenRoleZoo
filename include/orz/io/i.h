//
// Created by lby on 2018/1/15.
//

#ifndef ORZ_IO_I_H
#define ORZ_IO_I_H

#include "jug/binary.h"

#include <iostream>

namespace orz {
    binary read_file(const std::string &filename);
	std::string read_txt_file(const std::string &filename);

	class imemorystream : public std::istream {
	public:
		imemorystream(const char *data, size_t size) :
				std::istream(&m_buffer),
				m_buffer(data, size) {
			rdbuf(&m_buffer); // reset the buffer after it has been properly constructed
		}

	private:
		class imemorybuffer : public std::basic_streambuf<char> {
		public:
			imemorybuffer(const char *data, size_t size) {
				setg((char *) data, (char *) data, (char *) data + size);
			}
		};

		imemorybuffer m_buffer;
	};
}

#endif //ORZ_IO_I_H
