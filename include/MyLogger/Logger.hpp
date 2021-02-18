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

#ifndef MYLOGGER_LOGGER_HPP
#  define MYLOGGER_LOGGER_HPP

#  include "Singleton.tpp"
#  include "IFileLogger.hpp"
#  include "File.hpp"

namespace project {

// *****************************************************************************
//! \brief Project information. Automatically generated in build/ folder by
//! https://github.com/Lecrapouille/MyMakefile These information are stored in
//! the log file.
// *****************************************************************************
struct Info
{
    Info() = default;
    Info(bool const dbg, const char* name, uint32_t const major,
         uint32_t const minor, const char* sha1, const char* branch,
         const char* data, const char* tmp, const char* logname,
         const char* logpath)
        : debug(dbg),
          project_name(name),
          major_version(major),
          minor_version(minor),
          git_sha1(sha1),
          git_branch(branch),
          data_path(data),
          tmp_path(tmp),
          log_name(logname),
          log_path(logpath)
    {}

    //! \brief Project compiled in debug or released mode ?
    bool debug;
    //! \brief Project name (ie. "myproject").
    std::string project_name;
    //! \brief Major version of project (ie. 1).
    uint32_t major_version;
    //! \brief Minor version of project (ie. 0).
    uint32_t minor_version;
    //! \brief Save the git SHA1 (ie. "c9e5b68")
    std::string git_sha1;
    //! \brief Save the git branch (ie. "master")
    std::string git_branch;
    //! \brief Pathes where default project resources have been installed
    //! (when called  by the shell command: sudo make install. ie
    //! "/usr/share/myproject/data").
    std::string data_path;
    //! \brief Location for storing temporary files (ie. "/tmp/")
    std::string tmp_path;
    //! \brief Give the file name of the log file (ie. "myproject.log").
    std::string log_name;
    //! \brief Define the full path for the log (ie. "/tmp/myproject.log").
    std::string log_path;
};

} // namespace project

#ifndef SINGLETON_FOR_LOGGER
#  define SINGLETON_FOR_LOGGER LongLifeSingleton<Logger>
#endif

namespace tool { namespace log {

// *****************************************************************************
//! \brief Singleton file logger managing a single file.
// *****************************************************************************
class Logger: public IFileLogger, public SINGLETON_FOR_LOGGER
{
    friend class SINGLETON_FOR_LOGGER;

public:

    Logger() = default;

    //! \brief Open the log file defined inside the Info structure.
    //! \param info structure holding all project information (name, version ...)
    Logger(project::Info const& info);

    //! \brief Close and flush the log file.
    virtual ~Logger();

    //! \brief Reopen the log by changing of project (old file content is removed
    //! if file exists).
    //! \return true if successfuly opened.
    bool changeLog(project::Info const& info);

    //! \brief Reopen the log to new location (old content is removed if file exists).
    //! \return true if successfuly opened.
    bool changeLog(std::string const& filename);

protected:

    //! \brief Write a string inside the log file.
    virtual void write(std::string const& message) override
    {
        write(message.c_str());
    }

    //! \brief Write a string inside the log file.
    virtual void write(const char *message) override;

private:

    //! \brief Open the file.
    virtual bool open(std::string const& filename) override;

    //! \brief Close the file.
    virtual void close() override;

    //! \brief Write the header of the log file.
    virtual void header() override;

    //! \brief Write the footer of the log file.
    virtual void footer() override;

    //! \brief Get the current date (year, month, day). Store the date as string
    //! inside m_buffer_time.
    virtual const char* currentDate() override;

    //! \brief Get the current time (hour, minute, second). Store the date as
    //! string inside m_buffer_time.
    virtual const char* currentTime() override;

    //! \brief Entry point for logging function and line.
    virtual const char* funclineFormat() override;

    //! \brief Return the string name of the given severity
    virtual const char* severityToStr(const Severity& s) override;

    //! \brief Entry point for logging function and line.
    virtual const char* endOfLine() override;

private:

    project::Info m_info;
    std::ofstream m_file;
};

#define INIT_LOG(info) tool::log::Logger::instance().changeLog(info)

//! \brief std::endl shall be called to unlock
#define LOG_INSTANCE(sev)                                                   \
    tool::log::Logger::instance().lock()                                    \
                                 .time().severity(sev)                      \
                                 .funcline(__func__, __LINE__)

#define LOG_INSTANCE_FORMAT(sev, format, ...)                               \
    tool::log::Logger::instance().lock()                                    \
                                 .time().severity(sev)                      \
                                 .funcline(__func__, __LINE__)              \
                                 .log(format, __VA_ARGS__)                  \
                                 .unlock()

#define LOG_INSTANCE_FORMAT_LN(sev, format, ...)                            \
    tool::log::Logger::instance().lock()                                    \
                                 .time().severity(sev)                      \
                                 .funcline(__func__, __LINE__)              \
                                 .log(format, __VA_ARGS__)                  \
                                 .eol()                                     \
                                 .unlock()

#define LOG_INSTANCE_LIST(sev, list, del)                                   \
    tool::log::Logger::instance().lock()                                    \
                                 .time().severity(sev)                      \
                                 .funcline(__func__, __LINE__)              \
                                 .log(list, del)                            \
                                 .eol()                                     \
                                 .unlock()

#if defined(NDEBUG)
#    define LOGI(...) {} // Info
#    define LOGD(...) {} // Debug
#    define LOGW(...) {} // Warning
#    define LOGF(...) {} // Failure
#    define LOGE(...) {} // Error
#    define LOGS(...) {} // Signal
#    define LOGX(...) {} // Exception
#    define LOGC(...) {} // Catch
#    define LOGA(...) {} // Fatal
#else
#    define LOGI(...) LOG_INSTANCE_FORMAT_LN(tool::log::Info, __VA_ARGS__, "") // Info
#    define LOGD(...) LOG_INSTANCE_FORMAT_LN(tool::log::Debug, __VA_ARGS__, "") // Debug
#    define LOGW(...) LOG_INSTANCE_FORMAT_LN(tool::log::Warning, __VA_ARGS__, "") // Warning
#    define LOGF(...) LOG_INSTANCE_FORMAT_LN(tool::log::Failed, __VA_ARGS__, "") // Failure
#    define LOGE(...) LOG_INSTANCE_FORMAT_LN(tool::log::Error, __VA_ARGS__, "") // Error
#    define LOGS(...) LOG_INSTANCE_FORMAT_LN(tool::log::Signal, __VA_ARGS__, "") // Signal
#    define LOGX(...) LOG_INSTANCE_FORMAT_LN(tool::log::Exception, __VA_ARGS__, "") // Exception
#    define LOGC(...) LOG_INSTANCE_FORMAT_LN(tool::log::Catch, __VA_ARGS__, "") // Catch
#    define LOGA(...) LOG_INSTANCE_FORMAT_LN(tool::log::Fatal, __VA_ARGS__, "") // Fatal
#    define LOG(sev)  LOG_INSTANCE(sev)

#    define LOGE_IL(list, del) LOG_INSTANCE_LIST(tool::log::Error, (list), (del)) // Error
#endif

} } // namespace tool::log

#endif /* MYLOGGER_LOGGER_HPP */
