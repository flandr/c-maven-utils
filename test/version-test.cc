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

TEST(VersionTest, BasicComparison) {
    auto *v1 = mv_parse("1.0.0");
    auto *v2 = mv_parse("1.1.0");
    auto *v3 = mv_parse("2.0.1");

    ASSERT_EQ(0, mv_compare(v1, v1));
    ASSERT_EQ(0, mv_compare(v2, v2));
    ASSERT_EQ(0, mv_compare(v3, v3));

    ASSERT_EQ(-1, mv_compare(v1, v2));
    ASSERT_EQ(-1, mv_compare(v2, v3));
    ASSERT_EQ(-1, mv_compare(v1, v3));

    ASSERT_EQ(1, mv_compare(v2, v1));
    ASSERT_EQ(1, mv_compare(v3, v2));
    ASSERT_EQ(1, mv_compare(v3, v1));

    mv_free(v1);
    mv_free(v2);
    mv_free(v3);
}

::testing::AssertionResult assertVersionsEqual(const char* m_expr,
        const char* n_expr, const char *v1str, const char *v2str) {
    auto *v1 = mv_parse(v1str);
    auto *v2 = mv_parse(v2str);
    int cmp = mv_compare(v1, v2);
    free(v1);
    free(v2);

    if (!cmp) {
        return ::testing::AssertionSuccess();
    }

    return ::testing::AssertionFailure() << v1str << " != " << v2str;
}

::testing::AssertionResult assertVersionsOrder(const char* m_expr,
        const char* n_expr, const char *v1str, const char *v2str) {
    auto *v1 = mv_parse(v1str);
    auto *v2 = mv_parse(v2str);
    int cmp = mv_compare(v1, v2);
    free(v1);
    free(v2);

    if (cmp < 0) {
        return ::testing::AssertionSuccess();
    }

    return ::testing::AssertionFailure() <<  "! " << v1str << " < " << v2str;
}

void checkVersionsEqual(const char *v1str, const char *v2str) {
    ASSERT_PRED_FORMAT2(assertVersionsEqual, v1str, v2str);
}

void checkVersionsOrder(const char *v1str, const char *v2str) {
    ASSERT_PRED_FORMAT2(assertVersionsOrder, v1str, v2str);
}

TEST(VersionTest, Equality) {
    checkVersionsEqual( "1", "1" );
    checkVersionsEqual( "1", "1.0" );
    checkVersionsEqual( "1", "1.0.0" );
    checkVersionsEqual( "1.0", "1.0.0" );
    checkVersionsEqual( "1", "1-0" );
    checkVersionsEqual( "1", "1.0-0" );
    checkVersionsEqual( "1.0", "1.0-0" );
    // no separator between number and character
    checkVersionsEqual( "1a", "1-a" );
    checkVersionsEqual( "1a", "1.0-a" );
    checkVersionsEqual( "1a", "1.0.0-a" );
    checkVersionsEqual( "1.0a", "1-a" );
    checkVersionsEqual( "1.0.0a", "1-a" );
    checkVersionsEqual( "1x", "1-x" );
    checkVersionsEqual( "1x", "1.0-x" );
    checkVersionsEqual( "1x", "1.0.0-x" );
    checkVersionsEqual( "1.0x", "1-x" );
    checkVersionsEqual( "1.0.0x", "1-x" );

    // aliases
    checkVersionsEqual( "1ga", "1" );
    checkVersionsEqual( "1final", "1" );
    checkVersionsEqual( "1cr", "1rc" );

    // special "aliases" a, b and m for alpha, beta and milestone
    checkVersionsEqual( "1a1", "1-alpha-1" );
    checkVersionsEqual( "1b2", "1-beta-2" );
    checkVersionsEqual( "1m3", "1-milestone-3" );

    // case insensitive
    checkVersionsEqual( "1X", "1x" );
    checkVersionsEqual( "1A", "1a" );
    checkVersionsEqual( "1B", "1b" );
    checkVersionsEqual( "1M", "1m" );
    checkVersionsEqual( "1Ga", "1" );
    checkVersionsEqual( "1GA", "1" );
    checkVersionsEqual( "1Final", "1" );
    checkVersionsEqual( "1FinaL", "1" );
    checkVersionsEqual( "1FINAL", "1" );
    checkVersionsEqual( "1Cr", "1Rc" );
    checkVersionsEqual( "1cR", "1rC" );
    checkVersionsEqual( "1m3", "1Milestone3" );
    checkVersionsEqual( "1m3", "1MileStone3" );
    checkVersionsEqual( "1m3", "1MILESTONE3" );
}

TEST(VersionTest, Comparison) {
    checkVersionsOrder( "1", "2" );
    checkVersionsOrder( "1.5", "2" );
    checkVersionsOrder( "1", "2.5" );
    checkVersionsOrder( "1.0", "1.1" );
    checkVersionsOrder( "1.1", "1.2" );
    checkVersionsOrder( "1.0.0", "1.1" );
    checkVersionsOrder( "1.0.1", "1.1" );
    checkVersionsOrder( "1.1", "1.2.0" );

    checkVersionsOrder( "1.0-alpha-1", "1.0" );
    checkVersionsOrder( "1.0-alpha-1", "1.0-alpha-2" );
    checkVersionsOrder( "1.0-alpha-1", "1.0-beta-1" );

    checkVersionsOrder( "1.0-beta-1", "1.0-SNAPSHOT" );
    checkVersionsOrder( "1.0-SNAPSHOT", "1.0" );
    checkVersionsOrder( "1.0-alpha-1-SNAPSHOT", "1.0-alpha-1" );

    checkVersionsOrder( "1.0", "1.0-1" );
    checkVersionsOrder( "1.0-1", "1.0-2" );
    checkVersionsOrder( "1.0.0", "1.0-1" );

    checkVersionsOrder( "2.0-1", "2.0.1" );
    checkVersionsOrder( "2.0.1-klm", "2.0.1-lmn" );
    checkVersionsOrder( "2.0.1", "2.0.1-xyz" );

    checkVersionsOrder( "2.0.1", "2.0.1-123" );
    checkVersionsOrder( "2.0.1-xyz", "2.0.1-123" );
}
