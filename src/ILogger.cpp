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

#include "ILogger.hpp"
#include <cstring>

namespace tool { namespace log {

//------------------------------------------------------------------------------
ILogger::ILogger(size_t const buffer_size)
{
    m_buffer.resize(std::max(size_t(128), buffer_size));
}

//------------------------------------------------------------------------------
const char* ILogger::timeAndDateFormat(const char* format)
{
    static char buffer[32];

    time_t current_time = ::time(nullptr);
    strftime(buffer, sizeof (buffer), format, localtime(&current_time));
    return buffer;
}

//------------------------------------------------------------------------------
ILogger& ILogger::time()
{
    write(currentTime());

    return *this;
}

//------------------------------------------------------------------------------
ILogger& ILogger::severity(Severity const sev)
{
    write(severityToStr(sev));

    return *this;
}

//------------------------------------------------------------------------------
ILogger& ILogger::funcline(const char *function, int line)
{
    return log(funclineFormat(), function, line);
}

//------------------------------------------------------------------------------
ILogger& ILogger::eol()
{
    write(endOfLine());

    return *this;
}

//------------------------------------------------------------------------------
ILogger& ILogger::log(std::initializer_list<tool::log::Value> const& list, const char* del)
{
    std::ostringstream stream;
    for (auto& it: list)
        stream << it << del;

    stream.seekp(-int(strlen(del)), std::ios_base::end);
    write(stream.str());

    return *this;
}

//------------------------------------------------------------------------------
ILogger& ILogger::log(const char* format, ...)
{
    va_list params;
    va_start(params, format);
    int bytes = vsnprintf(m_buffer.data(), m_buffer.size(), format, params);
    va_end(params);

    // Remove '\n'
    size_t n = static_cast<size_t>(bytes);
    if ((bytes > 0) && (m_buffer[n - 1u] == '\n'))
    {
        m_buffer[n - 1u] = '\0';
    }

    write(m_buffer.data());

    return *this;
}

} } // namespace tool::log
