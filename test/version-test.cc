/*
 * Copyright (c) 2015 Nathan Rosenblum <flander@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <gtest/gtest.h>

#include "c-maven-utils/maven-version.h"

TEST(VersionTest, BasicParsing) {
    auto *v1 = mv_parse("3.2.1-7");
    ASSERT_EQ(3, mv_major(v1));
    ASSERT_EQ(2, mv_minor(v1));
    ASSERT_EQ(1, mv_incremental(v1));
    ASSERT_EQ(7, mv_build(v1));
    ASSERT_STREQ("", mv_qualifier(v1));
    mv_free(v1);

    v1 = mv_parse("3.2.1-SNAPSHOT");
    ASSERT_EQ(3, mv_major(v1));
    ASSERT_EQ(2, mv_minor(v1));
    ASSERT_EQ(1, mv_incremental(v1));
    ASSERT_EQ(-1, mv_build(v1));
    ASSERT_STREQ("SNAPSHOT", mv_qualifier(v1));
    mv_free(v1);
}
