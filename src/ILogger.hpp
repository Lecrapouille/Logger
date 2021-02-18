//=====================================================================
// MyLogger: A basic logger.
// Copyright 2018 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of MyLogger.
//
// MyLogger is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MyLogger.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef MYLOGGER_ILOGGER_HPP
#  define MYLOGGER_ILOGGER_HPP

#  include <mutex>
#  include <stdarg.h>
#  include <fstream>
#  include <sstream>
#  include <string>
#  include <iostream>
#  include <vector>

namespace tool { namespace log {

// *****************************************************************************
//! \brief Different severity enumerate
// *****************************************************************************
enum Severity
{
    None, Info, Debug, Warning, Failed, Error, Signal, Exception,
    Catch, Fatal, MaxLoggerSeverity = Fatal
};

// *****************************************************************************
//! \brief Singleton file logger managing a single file.
// *****************************************************************************
struct Value
{
private:

    union {
        float   f;
        double  d;
        ssize_t i;
        size_t  u;
        bool    b;
        const char* s;
    };

    enum { BOOL, FLOAT, DOUBLE, INT, UNSIGNED, STRING } tag;

public:

    Value(int i_)
        : i(i_), tag(Value::INT)
    {}

    Value(unsigned int u_)
        : u(u_), tag(Value::UNSIGNED)
    {}

    Value(float f_)
        : f(f_), tag(Value::FLOAT)
    {}

    Value(double d_)
        : d(d_), tag(Value::DOUBLE)
    {}

    Value(bool b_)
        : b(b_), tag(Value::BOOL)
    {}

    Value(const char* s_)
        : s(s_), tag(Value::STRING)
    {}

    inline friend std::ostream& operator<<(std::ostream& os, const Value& u)
{
    switch (u.tag)
    {
    case Value::INT: os << u.i; break;
    case Value::UNSIGNED: os << u.u; break;
    case Value::FLOAT: os << u.f; break;
    case Value::DOUBLE: os << u.d; break;
    case Value::BOOL: os << u.b; break;
    case Value::STRING: os << u.s; break;
    default: break;
    }
    return os;
}
};

// *****************************************************************************
//! \brief Interface class for loggers.
// *****************************************************************************
class ILogger
{
public:

    //! \brief Reserve the buffer (default: 1024 chars by line).
    ILogger(size_t const buffer_size = 1024u);

    //! \brief Virtual destructor because of virtual methods.
    virtual ~ILogger() = default;

    //! \brief entry point for logging data. This method formats data into
    //! m_buffer up to m_buffer.size() - 2 chars. (2: "\n\0").
    ILogger& log(const char* format, ...);

    //! \brief entry point for logging data. This method formats data into m_buffer.
    //! \tparam T type that can used by std::ostringstream << T.
    template <class T>
    ILogger& log(T const& data)
    {
        std::ostringstream stream;
        stream << data;
        write(stream.str());

        return *this;
    }

    //! \brief entry point for logging data. This method formats data into m_buffer.
    //! \tparam T type that can used by std::ostringstream << T.
    template <class T>
    ILogger& operator<<(T const& data)
    {
        return log<T>(data);
    }

    //! \brief 
    ILogger& operator<<(std::ostream& (*/*os*/)(std::ostream&))
    {
        m_mutex.unlock();
        return eol();
    }

    //! \brief entry point for logging data. This method formats initializer list
    //! into m_buffer.
    ILogger& log(std::initializer_list<tool::log::Value> const& list, const char* del = " ");

    //! \brief Entry point for logging current time.
    ILogger& time();

    //! \brief Entry point for logging severity.
    ILogger& severity(Severity const sev);

    //! \brief Entry point for logging function and line.
    ILogger& funcline(const char *function, int line);

    //! \brief Entry point for logging the end of log lines.
    ILogger& eol();

    //! \brief
    ILogger& lock()
    {
       m_mutex.lock();
       return *this;
    }

    //! \brief
    ILogger& unlock()
    {
       m_mutex.unlock();
       return *this;
    }

protected:

    //! \brief Return the date or time get by methods currentDate() or currentTime()
    const char* timeAndDateFormat(const char* format);

    //! \brief Virtual method used for storing m_buffer in the media you wish.
    virtual void write(std::string const& message) = 0;

    //! \brief Virtual method used for storing m_buffer in the media you wish.
    virtual void write(const char *message) = 0;

private:

    //! \brief Get the current date (year, month, day). Store the date as string
    //! inside m_buffer_time.
    virtual const char* currentDate() = 0;

    //! \brief Get the current time (hour, minute, second). Store the date as
    //! string inside m_buffer_time.
    virtual const char* currentTime() = 0;

    //! \brief Return the string name of the given severity
    virtual const char* severityToStr(const Severity& s) = 0;

    //! \brief Entry point for logging function and line.
    virtual const char* funclineFormat() = 0;

    //! \brief Return the end of line
    virtual const char* endOfLine() = 0;

protected:

    //! \brief Buffer used for storing a line of logs.
    std::vector<char> m_buffer;

    //! \brief Protect write against concurrency.
    std::mutex m_mutex;
};

} } // namespace tool::log

#endif /* MYLOGGER_ILOGGER_HPP */
