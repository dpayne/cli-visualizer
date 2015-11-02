#include <ncurses.h>
#include <iostream>
#include <gtest/gtest.h>
#include <Utils/NcursesUtils.h>

TEST(NcursesUtilsTest, ToColorEmptyStr)
{
    vis::ColorIndex color = vis::NcursesUtils::to_color_index("");
    EXPECT_EQ(COLOR_BLACK, color);
}

TEST(NcursesUtilsTest, ToColorName)
{
    EXPECT_EQ(COLOR_BLACK, vis::NcursesUtils::to_color_index("black"));
    EXPECT_EQ(COLOR_BLACK, vis::NcursesUtils::to_color_index("blAck"))
        << "to_color should be case insensitive";
    EXPECT_EQ(COLOR_BLUE, vis::NcursesUtils::to_color_index("blue"));
    EXPECT_EQ(COLOR_BLACK, vis::NcursesUtils::to_color_index("asdfasdf"));
    EXPECT_EQ(COLOR_BLACK, vis::NcursesUtils::to_color_index("100fasdf"));
}

TEST(NcursesUtilsTest, ToColorNumber)
{
    EXPECT_EQ(COLOR_WHITE, vis::NcursesUtils::to_color_index("7"));
    EXPECT_EQ(COLOR_BLUE, vis::NcursesUtils::to_color_index("4"));
    EXPECT_EQ(COLOR_BLACK, vis::NcursesUtils::to_color_index("0"));
    EXPECT_EQ(100, vis::NcursesUtils::to_color_index("100"));
}
