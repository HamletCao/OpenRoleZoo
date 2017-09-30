//
// Created by Lby on 2017/6/6.
//

#ifndef ORZ_UTILS_LOG_H
#define ORZ_UTILS_LOG_H

#include <iostream>
#include <sstream>
#include "format.h"
#include "except.h"

namespace orz {

    enum LogLevel {
        NONE = 0,
        DEBUG = 1,
        STATUS = 2,
        INFO = 3,
        FATAL = 4,
    };

    static LogLevel InnerGlobalLogLevel = STATUS;

    inline LogLevel GlobalLogLevel(LogLevel level) {
        LogLevel pre_level = InnerGlobalLogLevel;
        InnerGlobalLogLevel = level;
        return pre_level;
    }

    inline LogLevel GlobalLogLevel() {
        return InnerGlobalLogLevel;
    }

    class Log {
    public:
        Log(LogLevel level, std::ostream &log = std::cout)
                : m_level(level), m_log(log) {
        }

        ~Log() {
            std::string level_str = "Unkown";
            switch (m_level) {
                case NONE: return;
                case DEBUG: level_str = "DEBUG"; break;
                case STATUS: level_str = "STATUS"; break;
                case INFO: level_str = "INFO"; break;
                case FATAL: level_str = "FATAL"; break;
            }
            if (m_level >= InnerGlobalLogLevel) {
                auto msg = m_buffer.str();
                m_buffer.str("");
                m_buffer << level_str << ": " << msg;
                m_log << m_buffer.str() << std::endl;
            }
        }

        const std::string message() const {
            return m_buffer.str();
        }

        template<typename T>
        Log &operator()(const T &message) {
            if (m_level >= InnerGlobalLogLevel) {
                m_buffer << message;
            }
            return *this;
        }

        template<typename T>
        Log &operator<<(const T &message) {
            return operator()(message);
        }

        using Method = Log &(Log &);

        Log &operator<<(const Method &method) {
            if (m_level >= InnerGlobalLogLevel) {
                return method(*this);
            }
            return *this;
        }

    private:
        LogLevel m_level;
        std::ostringstream m_buffer;
        std::ostream &m_log;
    };

    inline Log &crash(Log &log)
    {
        throw Exception(log.message());
    }
}

#define ORZ_LOG(level) orz::Log(level)("[")(orz::Split(__FILE__, R"(/\)").back())(":")(__LINE__)("]: ")

#endif //ORZ_UTILS_LOG_H
