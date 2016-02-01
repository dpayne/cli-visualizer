#include <ncurses.h>
#include <iostream>
#include <gtest/gtest.h>
#include <Utils/Utils.h>

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
    std::unordered_map<std::string, std::string> m{
        {"a", "a"}, {"b", "b"}, {"c", "c"}};

    EXPECT_EQ(std::string{"a"},
              vis::Utils::get(m, std::string{"a"}, std::string{""}));
    EXPECT_EQ(std::string{"b"},
              vis::Utils::get(m, std::string{"b"}, std::string{""}));
    EXPECT_EQ(std::string{""},
              vis::Utils::get(m, std::string{"z"}, std::string{""}));
    EXPECT_EQ(std::string{"d"},
              vis::Utils::get(m, std::string{"z"}, std::string{"d"}));
}

TEST(UtilsTest, GetBoolFromStrStrUnorderedMap)
{
    std::unordered_map<std::string, std::string> m{
        {"a", "true"}, {"b", "false"}, {"c", "c"}};

    EXPECT_EQ(true, vis::Utils::get(m, std::string{"a"}, true));
    EXPECT_EQ(false, vis::Utils::get(m, std::string{"b"}, true));
    EXPECT_EQ(true, vis::Utils::get(m, std::string{"z"}, true));
    EXPECT_EQ(false, vis::Utils::get(m, std::string{"z"}, false));
}

TEST(UtilsTest, GetUintFromStrStrUnorderedMap)
{
    std::unordered_map<std::string, std::string> m{
        {"a", "1337"}, {"b", "42"}, {"c", "c"}};

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
