#include <Utils/Utils.h>
#include <Utils/NcursesUtils.h>
#include <gtest/gtest.h>
#include <iostream>
#include <ncurses.h>

TEST(UtilsTest, LowercaseAllLowerCase)
{
    std::string s{"hello world"};
    auto actual = vis::Utils::lowercase(s);
    std::string expected{"hello world"};

    EXPECT_EQ(expected, actual)
        << "lowercase must not change an all lowercased string";
}

TEST(UtilsTest, LowercaseAllUpperCase)
{
    std::string s{"HELLO WORLD"};
    auto actual = vis::Utils::lowercase(s);
    std::string expected{"hello world"};

    EXPECT_EQ(expected, actual)
        << "lowercase must lowercase an all uppercase string";
}

TEST(UtilsTest, LowercaseAllMixedCase)
{
    std::string s{"HeLlo wORLD"};
    auto actual = vis::Utils::lowercase(s);
    std::string expected{"hello world"};

    EXPECT_EQ(expected, actual)
        << "lowercase must lowercase a mixed case string";
}

TEST(UtilsTest, LowercaseEmptyString)
{
    std::string s{""};
    auto actual = vis::Utils::lowercase(s);
    std::string expected{""};

    EXPECT_EQ(expected, actual) << "lowercase must work with an empty string";
}

TEST(UtilsTest, ToBoolEmptyString)
{
    auto actual = vis::Utils::to_bool("");

    EXPECT_FALSE(actual) << "to_bool should return false for empty string";
}

TEST(UtilsTest, ToBoolOneString)
{
    auto actual = vis::Utils::to_bool("1");

    EXPECT_TRUE(actual) << "to_bool should return false for \"1\"";
}

TEST(UtilsTest, ToBoolZeroString)
{
    auto actual = vis::Utils::to_bool("0");

    EXPECT_FALSE(actual) << "to_bool should return false for \"0\"";
}

TEST(UtilsTest, ToBoolTrueString)
{
    auto actual = vis::Utils::to_bool("true");

    EXPECT_TRUE(actual) << "to_bool should return true for \"true\"";
}

TEST(UtilsTest, ToBoolTrUeString)
{
    auto actual = vis::Utils::to_bool("trUe");

    EXPECT_TRUE(actual) << "to_bool should return true for \"trUe\"";
}

TEST(UtilsTest, ToBoolFalseString)
{
    auto actual = vis::Utils::to_bool("false");

    EXPECT_FALSE(actual) << "to_bool should return true for \"false\"";
}

TEST(UtilsTest, ToBoolFaLseString)
{
    auto actual = vis::Utils::to_bool("faLse");

    EXPECT_FALSE(actual) << "to_bool should return true for \"faLse\"";
}

TEST(UtilsTest, GetStrFromStrStrUnorderedMap)
{
    std::unordered_map<std::string, std::wstring> m{
        {"a", L"a"}, {"b", L"b"}, {"c", L"c"}};

    EXPECT_EQ(std::wstring{L"a"},
              vis::Utils::get(m, std::string{"a"}, std::wstring{L""}));
    EXPECT_EQ(std::wstring{L"b"},
              vis::Utils::get(m, std::string{"b"}, std::wstring{L""}));
    EXPECT_EQ(std::wstring{L""},
              vis::Utils::get(m, std::string{"z"}, std::wstring{L""}));
    EXPECT_EQ(std::wstring{L"d"},
              vis::Utils::get(m, std::string{"z"}, std::wstring{L"d"}));
}

TEST(UtilsTest, GetBoolFromStrStrUnorderedMap)
{
    std::unordered_map<std::string, std::wstring> m{
        {"a", L"true"}, {"b", L"false"}, {"c", L"c"}};

    EXPECT_EQ(true, vis::Utils::get(m, std::string{"a"}, true));
    EXPECT_EQ(false, vis::Utils::get(m, std::string{"b"}, true));
    EXPECT_EQ(true, vis::Utils::get(m, std::string{"z"}, true));
    EXPECT_EQ(false, vis::Utils::get(m, std::string{"z"}, false));
}

TEST(UtilsTest, GetUintFromStrStrUnorderedMap)
{
    std::unordered_map<std::string, std::wstring> m{
        {"a", L"1337"}, {"b", L"42"}, {"c", L"c"}};

    EXPECT_EQ(1337,
              vis::Utils::get(m, std::string{"a"}, static_cast<uint32_t>(1)));
    EXPECT_EQ(42,
              vis::Utils::get(m, std::string{"b"}, static_cast<uint32_t>(1)));
    EXPECT_EQ(314,
              vis::Utils::get(m, std::string{"z"}, static_cast<uint32_t>(314)));
    EXPECT_EQ(0,
              vis::Utils::get(m, std::string{"z"}, static_cast<uint32_t>(0)));
}

TEST(UtilsTest, SplitFirstEmptyString)
{
    std::pair<std::string, std::string> p{"hello", "world"};
    std::string s{""};

    vis::Utils::split_first(s, '=', p);

    EXPECT_EQ("", p.first);
    EXPECT_EQ("", p.second);
}

TEST(UtilsTest, SplitFirstSingleDelimString)
{
    std::pair<std::string, std::string> p{"hello", "world"};
    std::string s{"this=isatest"};

    vis::Utils::split_first(s, '=', p);

    EXPECT_EQ("this", p.first);
    EXPECT_EQ("isatest", p.second);
}

TEST(UtilsTest, SplitFirstMultiDelimString)
{
    std::pair<std::string, std::string> p{"hello", "world"};
    std::string s{"this=isatest=with=more=than=one=delimiter"};

    vis::Utils::split_first(s, '=', p);

    EXPECT_EQ("this", p.first);
    EXPECT_EQ("isatest=with=more=than=one=delimiter", p.second);
}

TEST(UtilsTest, SplitEmptyString)
{
    std::vector<std::string> v{"hello", "world"};
    std::string s{""};

    vis::Utils::split(s, '=', v);

    EXPECT_TRUE(v.empty());
}

TEST(UtilsTest, SplitSingleDelimString)
{
    std::vector<std::string> v{"hello", "world"};
    std::string s{"this=isatest"};

    vis::Utils::split(s, '=', v);

    EXPECT_EQ(2, v.size());
    EXPECT_EQ("this", v[0]);
    EXPECT_EQ("isatest", v[1]);
}

TEST(UtilsTest, SplitMultiDelimString)
{
    std::vector<std::string> v{"hello", "world"};
    std::string s{"this=isatest=with=more=than=one=delimiter"};

    vis::Utils::split(s, '=', v);

    EXPECT_EQ(7, v.size());
    EXPECT_EQ("this", v[0]);
    EXPECT_EQ("isatest", v[1]);
    EXPECT_EQ("with", v[2]);
    EXPECT_EQ("more", v[3]);
    EXPECT_EQ("than", v[4]);
    EXPECT_EQ("one", v[5]);
    EXPECT_EQ("delimiter", v[6]);
}

TEST(UtilsTest, ToIntEmptyString)
{
    EXPECT_EQ(0, vis::Utils::to_int(""));
}

TEST(UtilsTest, ToIntMinInt)
{
    EXPECT_EQ(-2147483648, vis::Utils::to_int("-2147483648"));
}

TEST(UtilsTest, ToIntMaxInt)
{
    EXPECT_EQ(2147483647, vis::Utils::to_int("2147483647"));
}

TEST(UtilsTest, ToIntZero)
{
    EXPECT_EQ(0, vis::Utils::to_int("0"));
}

TEST(UtilsTest, ToUIntEmptyString)
{
    EXPECT_EQ(0, vis::Utils::to_uint(""));
}

TEST(UtilsTest, ToUIntMaxInt)
{
    EXPECT_EQ(4294967295, vis::Utils::to_uint("4294967295"));
}

TEST(UtilsTest, ToUInt)
{
    EXPECT_EQ(1337, vis::Utils::to_uint("1337"));
}

TEST(UtilsTest, ToUIntZero)
{
    EXPECT_EQ(0, vis::Utils::to_uint("0"));
}

TEST(UtilsTest, 24bitColor)
{
    std::cout << "\x1b[38;2;255;100;0mTRUECOLOR\n" << std::endl;
    std::cout << "TRUECOLOR\n" << std::endl;
    std::cout << "\x1b[0m\n" << std::endl;

    for (const auto &color : VisConstants::k_default_colors)
    {
        vis::ColorIndex colorIndex = color.get_color_index();
        for (vis::ColorIndex red = 0; red <= 255; ++red)
        {
            for (vis::ColorIndex green = 0; green <= 255; ++green)
            {
                for (vis::ColorIndex blue = 0; blue <= 255; ++blue)
                {
                    if (colorIndex ==
                        vis::NcursesUtils::to_ansi_color(red, green, blue))
                    {
                        // std::cout << "\x1b[38;2;" << red << ";" << green <<
                        // ";" << blue << "mTRUECOLOR\x1b[0m";
                        std::cout << "vis::ColorDefinition{" << colorIndex
                                  << " ," << red << " ," << blue << " ,"
                                  << green << "},";
                        goto found_color;
                    }
                }
            }
        }
    found_color:
        std::cout << std::endl;
    }
}

#define VIS_A_COLOR (NCURSES_BITS(((1U) << 16) - 1U, 0))
#define VIS_COLOR_PAIR(n) (NCURSES_BITS((n), 0) & VIS_A_COLOR)

TEST(UtilsTest, Ncurses24bitColor)
{
    initscr();
    noecho();    // disable printing of pressed keys
    curs_set(0); // sets the cursor to invisible
    setlocale(LC_ALL, "");
    start_color();        // turns on color
    use_default_colors(); // uses default colors of terminal, which allows

    // initialize colors
    for (int16_t i = 0; i < COLORS; ++i)
    {
        init_pair(i, i, -1);

        // initialize colors as background, this is used in write_background to
        // create a
        // full block effect without using a custom font
        init_pair(static_cast<int16_t>(i + COLORS), i, i);
    }

    for ( size_t j = 0; j < VisConstants::k_default_colors.size(); ++j)
    {
        const auto color = VisConstants::k_default_colors[j];
        const auto color_pair =
            VIS_COLOR_PAIR(color.get_color_index() + COLORS);

        attron(color_pair);

        for (int i = 0; i < 100; ++i)
        {
            mvaddwstr(j, i, L"#");
        }

        attroff(color_pair);
    }

    refresh();

    auto test_colors = COLORS;

    endwin();
    sleep(1);
    std::cerr << "Colors: " << test_colors << std::endl;
}
