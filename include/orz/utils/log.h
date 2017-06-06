//
// Created by Lby on 2017/6/6.
//

#ifndef ORZ_UTILS_LOG_H
#define ORZ_UTILS_LOG_H

#include <iostream>
#include "format.h"

namespace orz
{
    enum LogLevel
    {
        DEBUG = 1,
        STATUS = 2,
        INFO = 3,
        ERROR = 4,
    };

    static LogLevel InnerGlobalLogLevel = STATUS;

    LogLevel GlobalLogLevel(LogLevel level)
    {
        LogLevel pre_level = InnerGlobalLogLevel;
        InnerGlobalLogLevel = level;
        return pre_level;
    }

    LogLevel GlobalLogLevel()
    {
        return InnerGlobalLogLevel;
    }

    class Log
    {
    public:
        Log(LogLevel level, std::ostream &log = std::cout)
            : m_level(level), m_log(log)
        {
            switch (m_level)
            {
                case DEBUG:
                    *this << "DEBUG: ";
                    break;
                case STATUS:
                    *this << "STATUS: ";
                    break;
                case INFO:
                    *this << "INFO: ";
                    break;
                case ERROR:
                    *this << "ERROR: ";
                    break;
            }
        }

        ~Log()
        {
            if (m_level >= InnerGlobalLogLevel) m_log << std::endl;
        }

        template <typename T>
        Log &operator ()(const T&message)
        {
            if (m_level >= InnerGlobalLogLevel)
            {
                m_log << message;
            }
            return *this;
        }

        template <typename T>
        Log &operator <<(const T &message)
        {
            return operator()(message);
        }

    private:
        LogLevel m_level;
        std::ostream &m_log;
    };
}

#define RichLog(level) orz::Log(level)("[")(orz::Split(__FILE__, R"(/\)").back())(":")(__LINE__)("] ")

#endif //ORZ_UTILS_LOG_H
