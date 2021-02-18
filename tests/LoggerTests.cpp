//=====================================================================
// MyLogger: A GIS in a spreadsheet.
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

#include "main.hpp"
#include <thread>
#include <algorithm>
#include <iterator>
#include <cstring>

#define SINGLETON_FOR_LOGGER Singleton<Logger>

#define protected public
#define private public
#  include "MyLogger/Logger.hpp"
#undef protected
#undef private

#include "project_info.hpp"

using namespace tool::log;
static const uint32_t header_footer_lines = 6U /* header */ + 3U /* footer */;

//--------------------------------------------------------------------------
static uint32_t number_of_lines(std::string const& file)
{
    uint32_t line_count = 0U;
    std::ifstream myfile(file);
    if (!myfile)
    {
        std::cerr << "Could not open log '" << file << "' Reason: '"
                  << strerror(errno) << "'" << std::endl;
        return 0u;
    }

    // New lines will be skipped unless we stop it from happening:
    myfile.unsetf(std::ios_base::skipws);

    // Count the newlines with an algorithm specialized for counting:
    line_count = uint32_t(std::count(
                              std::istream_iterator<char>(myfile),
                              std::istream_iterator<char>(), '\n'));

    return line_count;
}

//--------------------------------------------------------------------------
static void call_from_thread(uint32_t const x, uint32_t const lines_by_thread)
{
    for (uint32_t i = 0U; i < lines_by_thread; ++i)
    {
        LOGI("Hello World from thread %3u ", x);
    }
}

//--------------------------------------------------------------------------
TEST(LoggerTests, testEmptyLog)
{
    INIT_LOG(project::info);

    constexpr uint32_t num_threads = 0U;
    constexpr uint32_t lines_by_thread = 0U;

    EXPECT_THAT(true, Logger::instance().changeLog("/tmp/empty.log"));
    ASSERT_STREQ(Logger::instance().m_info.log_name.c_str(), "empty.log");
    ASSERT_STREQ(Logger::instance().m_info.log_path.c_str(), "/tmp/empty.log");
    Logger::destroy();

    uint32_t lines = number_of_lines("/tmp/empty.log");
    ASSERT_EQ(num_threads * lines_by_thread + header_footer_lines, lines);
}

//--------------------------------------------------------------------------
TEST(LoggerTests, testWithInitializerList)
{
    EXPECT_THAT(true, Logger::instance().changeLog("/tmp/initlist.log"));
    tool::log::Logger::instance().lock()
                                 .time().severity(tool::log::Error)
                                 .funcline(__func__, __LINE__)
                                 .log({"hello world", 66, 67u, 55.0f, 45.0, false, 'a'}, "--")
                                 .log({"hello world", 66, 67u, 55.0f, 45.0, false, 'a'}, "+")
                                 .eol()
                                 .unlock();
}

//--------------------------------------------------------------------------
TEST(LoggerTests, testBasic)
{
    {
        constexpr uint32_t num_threads = 1U;
        constexpr uint32_t lines_by_thread = 2U;

        // Change log with a full path
        INIT_LOG(project::info);
        EXPECT_THAT(true, Logger::instance().changeLog("/tmp/foo/bar/new.log"));
        ASSERT_STREQ(Logger::instance().m_info.log_name.c_str(), "new.log");
        ASSERT_STREQ(Logger::instance().m_info.log_path.c_str(), "/tmp/foo/bar/new.log");
        LOG(Fatal) << "test" << std::endl;
        LOGE("%s %u", "Coucou", 42u);
        Logger::destroy();
        uint32_t lines = number_of_lines("/tmp/foo/bar/new.log");
        ASSERT_EQ(num_threads * lines_by_thread + header_footer_lines, lines);
    }
    {
        constexpr uint32_t num_threads = 1U;
        constexpr uint32_t lines_by_thread = 3U;

        // Change log with a simple file name
        EXPECT_THAT(true, Logger::instance().changeLog("simplefile.log"));
        ASSERT_STREQ(Logger::instance().m_info.log_name.c_str(), "simplefile.log");
        ASSERT_STREQ(Logger::instance().m_info.log_path.c_str(), "simplefile.log");
        LOG(Warning) << "test2" << std::endl;
        LOGD("%s", "Coucou");
        LOGW("%s %f %d", "TEST", 42.0f, 66);
        Logger::destroy();
        uint32_t lines = number_of_lines("simplefile.log");
        ASSERT_EQ(num_threads * lines_by_thread + header_footer_lines, lines);
    }
}

//--------------------------------------------------------------------------
TEST(LoggerTests, testWithConcurrency)
{
    constexpr uint32_t num_threads = 10U;
    constexpr uint32_t lines_by_thread = 10U;

    Logger::instance().changeLog(project::info);

    static std::thread t[num_threads];

    // Launch a group of threads
    for (uint32_t i = 0; i < num_threads; ++i)
    {
        t[i] = std::thread(call_from_thread, i, lines_by_thread);
    }

    // Join the threads with the main thread
    for (uint32_t i = 0; i < num_threads; ++i)
    {
        if (t[i].joinable())
        {
            t[i].join();
        }
    }
    Logger::destroy();

    // Check: count the number of lines == num_threads + header + footer
    uint32_t lines = number_of_lines(project::info.log_path);
    ASSERT_EQ(num_threads * lines_by_thread + header_footer_lines, lines);
}
