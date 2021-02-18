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

#define protected public
#define private public
#  include "MyLogger/Path.hpp"
#undef protected
#undef private

using ::testing::HasSubstr;

//--------------------------------------------------------------------------
TEST(PathTests, EmptyConstructor)
{
    Path path;

    ASSERT_EQ(0u, path.m_search_paths.size());
    ASSERT_EQ(':', path.m_delimiter);
    ASSERT_EQ(true, path.m_string_path == "");
}

//--------------------------------------------------------------------------
TEST(PathTests, SplitConstructor)
{
    Path path("/a/b:c/d");

    ASSERT_EQ(2, path.m_search_paths.size());
    ASSERT_STREQ(path.toString().c_str(), ".:/a/b:c/d:");
    path.clear();
    ASSERT_EQ(0, path.m_search_paths.size());
    ASSERT_STREQ(path.toString().c_str(), "");
    path.add("g/g");
    ASSERT_EQ(1, path.m_search_paths.size());
    ASSERT_STREQ(path.toString().c_str(), ".:g/g:");
    path.reset("a/b");
    ASSERT_EQ(1, path.m_search_paths.size());
    ASSERT_STREQ(path.toString().c_str(), ".:a/b:");
    std::cout << path.toString() << std::endl;
}

//--------------------------------------------------------------------------
TEST(PathTests, SplitDir)
{
    Path path;
    path.split("/a//b\\d/:e\\d:");
    ASSERT_STREQ(path.toString().c_str(), ".:/a//b\\d:e\\d:");
    ASSERT_EQ(2, path.m_search_paths.size());
    path.remove("incorrect/path");
    ASSERT_EQ(2, path.m_search_paths.size());
    path.remove("/a//b\\d");
    ASSERT_EQ(2, path.m_search_paths.size()); // FIXME should be1
    path.remove("/a//b\\d/");
    ASSERT_EQ(1, path.m_search_paths.size());
    ASSERT_STREQ(path.toString().c_str(), ".:e\\d:");
    path.remove("e\\d/");
    ASSERT_EQ(0, path.m_search_paths.size());
    path.remove("");
    ASSERT_EQ(0, path.m_search_paths.size());
    path.remove("incorrect/path");
    ASSERT_EQ(0, path.m_search_paths.size());
    path.add("g/g");
    ASSERT_EQ(1, path.m_search_paths.size());
    ASSERT_STREQ(path.toString().c_str(), ".:g/g:");
}

//--------------------------------------------------------------------------
TEST(PathTests, FindAndExpand)
{
    Path path("/bin:/usr/bin:/usr/local/bin");
    EXPECT_THAT(path.expand("ls").c_str(), HasSubstr("/bin/ls"));
    auto res1 = path.find("ls");

    ASSERT_EQ(true, res1.second);
    EXPECT_THAT(res1.first.c_str(), HasSubstr("/bin/ls"));

    auto res3 = path.find(res1.first);
    ASSERT_EQ(true, res3.second);
    ASSERT_STREQ(res3.first.c_str(), res1.first.c_str());

    path.clear();
    ASSERT_EQ(true, path.expand("ls") == "ls");
    auto res2 = path.find("ls");
    ASSERT_EQ(false, res2.second);
    ASSERT_STREQ(res2.first.c_str(), "");
}
