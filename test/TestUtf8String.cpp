
#include <iostream>
#include <gtest/gtest.h>
#include "tlib/String.hpp"
#include <boost/locale.hpp>
#include <locale>

TEST(Utf8StringTest, characters0) {
    boost::locale::generator gen;
    std::locale::global(gen(""));
    
    tlib::U8String str("Hello World!");
    
    EXPECT_EQ(12, str.characters());
    EXPECT_EQ(12, str.length());
    EXPECT_EQ(12, str.size());
}

TEST(Utf8StringTest, characters1) {
    boost::locale::generator gen;
    std::locale::global(gen(""));
    
    tlib::U8String str("±");
    EXPECT_EQ(1, str.characters()); //< 1 character
    EXPECT_EQ(1, str.length());     //< 1 codepoint
    EXPECT_EQ(2, str.size());       //< 2 bytes
}

TEST(Utf8StringTest, characters2) {
    boost::locale::generator gen;
    std::locale::global(gen(""));
    
    tlib::U8String str("\u0800");
    EXPECT_EQ(1, str.characters());
    EXPECT_EQ(1, str.length());
    EXPECT_EQ(3, str.size());
}

TEST(Utf8StringTest, combining) {
    boost::locale::generator gen;
    std::locale::global(gen(""));
    
    // Combining mark after the letter
    // Combining characters don't increase the number of characters in the
    // string:
    //   characters() == 1
    //
    // This can't normalize to a single codepoint and so remains 2 codepoints
    //   length() == 2
    //
    // The byte representation of \u0333 is 2 bytes, and that of e is 1 byte
    //   size() == 3
    tlib::U8String str("e\u0333");
    
    EXPECT_EQ(1, str.characters());
    EXPECT_EQ(2, str.length());
    EXPECT_EQ(3, str.size());
}
