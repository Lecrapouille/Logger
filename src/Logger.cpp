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

#include "MyLogger/Logger.hpp"
#include "MyLogger/File.hpp"
#include <cstring>

namespace tool { namespace log {

//------------------------------------------------------------------------------
Logger::Logger(project::Info const& info)
    : m_info(info)
{
    open(m_info.log_path);
}

//------------------------------------------------------------------------------
Logger::~Logger()
{
    close();
}

//------------------------------------------------------------------------------
bool Logger::changeLog(project::Info const& info)
{
    close();
    m_info = info;
    return open(m_info.log_path);
}

//------------------------------------------------------------------------------
bool Logger::changeLog(std::string const& logpath)
{
    close();
    m_info.log_path = logpath;
    m_info.log_name = File::fileName(logpath);
    if (m_info.log_name.empty())
    {
        m_info.log_name = "log.txt";
    }
    return open(m_info.log_path);
}

//------------------------------------------------------------------------------
bool Logger::open(std::string const& logfile)
{
    // Distinguish behavior between simple file and absolute path.
    std::string dir = File::dirName(logfile);
    std::string file(logfile);
    if (dir.empty())
    {
        dir = m_info.tmp_path;
        file = dir + file;
    }

    // Call it before Logger constructor
    if (!File::mkdir(dir))
    {
        std::cerr << "Failed creating the temporary directory '"
                  << m_info.tmp_path << "'" << std::endl;
        return false;
    }

    // Try to open the given log path
    m_file.open(file.c_str());
    if (!m_file)
    {
        std::cerr << "Failed creating the log file '"
                  << file << "'. Reason is '"
                  << strerror(errno) << "'"
                  << std::endl;
        return false;
    }
    else
    {
        std::cout << "Log created: '" << file
                  << "'" << std::endl << std::endl;
        header();
    }
    return true;
}

//------------------------------------------------------------------------------
void Logger::close()
{
    if (!m_file)
        return ;

    footer();
    m_file.close();
}

//------------------------------------------------------------------------------
void Logger::write(const char *message)
{
    if (!m_file)
        return ;

    m_file << message;
    m_file.flush();
}

//------------------------------------------------------------------------------
const char* Logger::severityToStr(const Severity& s)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    static const char *c_severities[] =
    {
        [Severity::None]      = "",
        [Severity::Info]      = "[INFO]",
        [Severity::Debug]     = "[DEBUG]",
        [Severity::Warning]   = "[WARNING]",
        [Severity::Failed]    = "[FAILURE]",
        [Severity::Error]     = "[ERROR]",
        [Severity::Signal]    = "[SIGNAL]",
        [Severity::Exception] = "[THROW]",
        [Severity::Catch]     = "[CATCH]",
        [Severity::Fatal]     = "[FATAL]"
    };
#pragma GCC diagnostic pop
    return c_severities[s];
}

//------------------------------------------------------------------------------
const char* Logger::currentDate()
{
    return timeAndDateFormat("[%Y/%m/%d]");
}

//------------------------------------------------------------------------------
const char* Logger::currentTime()
{
    return timeAndDateFormat("[%H:%M:%S]");
}

//------------------------------------------------------------------------------
const char* Logger::funclineFormat()
{
    return "[%s::%d] ";
}

//------------------------------------------------------------------------------
const char* Logger::endOfLine()
{
    return "\n";
}

//------------------------------------------------------------------------------
void Logger::header()
{
    log("======================================================\n"
        "  %s %s %u.%u - Event log - %s\n"
        "  git branch: %s\n"
        "  git SHA1: %s\n"
        "======================================================\n\n\n",
        m_info.project_name.c_str(),
        m_info.debug ? "Debug" : "Release",
        m_info.major_version,
        m_info.minor_version,
        currentDate(),
        m_info.git_branch.c_str(),
        m_info.git_sha1.c_str());
}

//------------------------------------------------------------------------------
void Logger::footer()
{
    log("\n======================================================\n"
        "  %s log closed at %s\n"
        "======================================================\n",
        m_info.project_name.c_str(),
        currentTime());
}

} } // namespace tool::log
