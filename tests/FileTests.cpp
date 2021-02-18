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
#include "MyLogger/File.hpp"

//--------------------------------------------------------------------------
TEST(FileTests, PathManipulation)
{
    std::string res;

    res = File::fileName("/home/qq/MyLogger/tests/common/FileTests.cpp");
    ASSERT_STREQ(res.c_str(), "FileTests.cpp");

    res = File::fileName("/home/qq/MyLogger/tests/common");
    ASSERT_STREQ(res.c_str(), "common");

    res = File::fileName("common");
    ASSERT_STREQ(res.c_str(), "common");

    res = File::baseName("/home/qq/MyLogger/tests/common/FileTests.cpp");
    ASSERT_STREQ(res.c_str(), "FileTests");

    res = File::baseName("/home/qq/MyLogger/tests/common");
    ASSERT_STREQ(res.c_str(), "common");

    res = File::baseName("common");
    ASSERT_STREQ(res.c_str(), "common");

    res = File::extension("/home/qq/MyLogger/tests/common/FileTests.cpp");
    ASSERT_STREQ(res.c_str(), "cpp");

    res = File::extension("/home/qq/MyLogger/tests/common/FileTests.cpp~");
    ASSERT_STREQ(res.c_str(), "cpp");

    res = File::extension("/home/qq/MyLogger/tests/common/FileTests.cpp.hpp");
    ASSERT_STREQ(res.c_str(), "hpp");

    res = File::extension("/home/qq/MyLogger/tests/common");
    ASSERT_STREQ(res.c_str(), "");

    res = File::extension("common");
    ASSERT_STREQ(res.c_str(), "");
    ASSERT_EQ(false, File::exist("pouet"));
    ASSERT_EQ(true, File::exist("/dev/null"));

    ASSERT_EQ(true, File::Directory == File::type("/tmp"));
    ASSERT_EQ(true, File::isReadable("/tmp"));
    ASSERT_EQ(true, File::isWritable("/tmp"));

    if (File::exist("/usr/sbin/")) // FIXME: does not exist on Travis-CI docker
    {
        ASSERT_EQ(true, File::Directory == File::type("/usr/sbin/"));
        ASSERT_EQ(true, File::isReadable("/usr/sbin/"));
        //FIXME ASSERT_EQ(true, false == File::isWritable("/usr/sbin/"));
    }
    ASSERT_EQ(true, File::DoesNotExist == File::type("/usr/sbin/foobar"));
    ASSERT_EQ(false, File::isReadable("/usr/sbin/foobar"));
    ASSERT_EQ(false, File::isWritable("/usr/sbin/foobar"));

    ASSERT_EQ(true, File::Document == File::type("/bin/ls"));
    ASSERT_EQ(true, File::isReadable("/bin/ls"));
    //FIXME ASSERT_EQ(true, false == File::isWritable("/bin/ls"));

    ASSERT_EQ(true, File::mkdir("/tmp/qq"));
    ASSERT_EQ(true, File::mkdir("/tmp/foo/bar/"));
    ASSERT_EQ(true, File::mkdir("/tmp/foo/bar/"));

    ASSERT_EQ(false, File::mkdir("/dev/null"));

    ASSERT_EQ(true, File::dirName("/tmp/foo/bar") == "/tmp/foo/");
    ASSERT_EQ(true, File::dirName("/tmp/foo/") == "/tmp/foo/");
    ASSERT_EQ(true, File::dirName("/tmp/foo") == "/tmp/");
    ASSERT_EQ(true, File::dirName("/tmp/") == "/tmp/");
    ASSERT_EQ(true, File::dirName("/tmp") == "/");

    ASSERT_EQ(true, File::dirName("tmp/foo") == "tmp/");
    ASSERT_EQ(true, File::dirName("tmp/foo/") == "tmp/foo/");
    ASSERT_EQ(true, File::dirName("tmp") == "");
}
