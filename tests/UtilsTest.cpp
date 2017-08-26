#include <Utils/NcursesUtils.h>
#include <Utils/Utils.h>
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

    vis::Utils::split_first(s, '=', &p);

    EXPECT_EQ("", p.first);
    EXPECT_EQ("", p.second);
}

TEST(UtilsTest, SplitFirstSingleDelimString)
{
    std::pair<std::string, std::string> p{"hello", "world"};
    std::string s{"this=isatest"};

    vis::Utils::split_first(s, '=', &p);

    EXPECT_EQ("this", p.first);
    EXPECT_EQ("isatest", p.second);
}

TEST(UtilsTest, SplitFirstMultiDelimString)
{
    std::pair<std::string, std::string> p{"hello", "world"};
    std::string s{"this=isatest=with=more=than=one=delimiter"};

    vis::Utils::split_first(s, '=', &p);

    EXPECT_EQ("this", p.first);
    EXPECT_EQ("isatest=with=more=than=one=delimiter", p.second);
}

TEST(UtilsTest, SplitEmptyString)
{
    std::vector<std::string> v{"hello", "world"};
    std::string s{""};

    vis::Utils::split(s, '=', &v);

    EXPECT_TRUE(v.empty());
}

TEST(UtilsTest, SplitSingleDelimString)
{
    std::vector<std::string> v{"hello", "world"};
    std::string s{"this=isatest"};

    vis::Utils::split(s, '=', &v);

    EXPECT_EQ(2, v.size());
    EXPECT_EQ("this", v[0]);
    EXPECT_EQ("isatest", v[1]);
}

TEST(UtilsTest, SplitMultiDelimString)
{
    std::vector<std::string> v{"hello", "world"};
    std::string s{"this=isatest=with=more=than=one=delimiter"};

    vis::Utils::split(s, '=', &v);

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

TEST(UtilsTest, IsNumericZero)
{
    EXPECT_TRUE(vis::Utils::is_numeric("0"));
}

TEST(UtilsTest, IsNumericOne)
{
    EXPECT_TRUE(vis::Utils::is_numeric("1"));
}

TEST(UtilsTest, IsNumericNegative)
{
    EXPECT_TRUE(vis::Utils::is_numeric("-1"));
}

TEST(UtilsTest, IsNumericMaxInt)
{
    EXPECT_TRUE(vis::Utils::is_numeric("2147483647"));
}

TEST(UtilsTest, IsNumericLetter)
{
    EXPECT_FALSE(vis::Utils::is_numeric("A"));
}

TEST(UtilsTest, IsNumericWords)
{
    EXPECT_FALSE(vis::Utils::is_numeric("hello world"));
}
