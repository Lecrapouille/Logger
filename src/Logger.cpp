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

#ifdef _WIN32
// FIXME: weak or undefined references are not supported on Windows DLLs
// while accepted by Linux. See GitHub ticket #1.
namespace config
{
  //! \brief Compiled in debug or released mode
  bool debug;
  //! \brief Used for logs and GUI.
  std::string project_name;
  //! \brief Major version of project
  uint32_t major_version;
  //! \brief Minor version of project
  uint32_t minor_version;
  //! \brief Save the git SHA1
  std::string git_sha1;
  //! \brief Save the git branch
  std::string git_branch;
  //! \brief Pathes where default project resources have been installed
  //! (when called  by the shell command: sudo make install).
  std::string data_path;
  //! \brief Location for storing temporary files
  std::string tmp_path;
  //! \brief Give a name to the default project log file.
  std::string log_name;
  //! \brief Define the full path for the project.
  std::string log_path;
}
#endif

namespace tool { namespace log {

//------------------------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
static const char *c_str_severity[Severity::MaxLoggerSeverity + 1] =
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

//------------------------------------------------------------------------------
Logger::Logger(std::string const& filename)
{
    open(filename);
}

//------------------------------------------------------------------------------
Logger::Logger()
{
    open(config::log_path);
}

//------------------------------------------------------------------------------
Logger::~Logger()
{
    close();
}

//------------------------------------------------------------------------------
bool Logger::changeLog(std::string const& logfile)
{
    close();
    return open(logfile);
}

//------------------------------------------------------------------------------
bool Logger::open(std::string const& logfile)
{
    // Distinguish behavior between simple file and absolute path.
    std::string dir = File::dirName(logfile);
    std::string file(logfile);
    if (dir.empty())
    {
        dir = config::tmp_path;
        file = dir + file;
    }

    // Call it before Logger constructor
    if (!File::mkdir(dir))
    {
        std::cerr << "Failed creating the temporary directory '"
                  << config::tmp_path << "'" << std::endl;
        return false;
    }

    // Try to open the given log path
    m_file.open(file.c_str());
    if (!m_file.is_open())
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
    if (!m_file.is_open())
        return ;

    footer();
    m_file.close();
}

//------------------------------------------------------------------------------
void Logger::write(const char *message, const int /*length*/)
{
    if (nullptr != m_stream)
    {
        (*m_stream) << message;
        m_stream->flush();
    }

    if (!m_file.is_open())
        return ;

    m_file << message;
    m_file.flush();
}

//------------------------------------------------------------------------------
void Logger::beginOfLine()
{
    currentTime();
    write(m_buffer_time);
    write(c_str_severity[m_severity]);
}

//------------------------------------------------------------------------------
void Logger::header()
{
    currentDate();
    log("======================================================\n"
        "  %s %s %u.%u - Event log - %s\n"
        "  git branch: %s\n"
        "  git SHA1: %s\n"
        "======================================================\n\n",
        config::project_name.c_str(),
        config::debug ? "Debug" : "Release",
        config::major_version,
        config::minor_version,
        m_buffer_time,
        config::git_branch.c_str(),
        config::git_sha1.c_str());
}

//------------------------------------------------------------------------------
void Logger::footer()
{
    currentTime();
    log("\n======================================================\n"
        "  %s log closed at %s\n"
        "======================================================\n\n",
        config::project_name.c_str(),
        m_buffer_time);
}

//------------------------------------------------------------------------------
ILogger& Logger::operator<<(const Severity& severity)
{
    write(c_str_severity[severity]);
    return *this;
}

//------------------------------------------------------------------------------
ILogger& Logger::operator<<(const char *msg)
{
    write(msg);
    return *this;
}

} } // namespace tool::log
