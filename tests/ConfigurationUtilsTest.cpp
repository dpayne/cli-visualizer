#include <Utils/ConfigurationUtils.h>
#include <Utils/Utils.h>
#include <gtest/gtest.h>
#include <iostream>
#include <ncurses.h>

namespace vis
{
class ConfigurationUtilsTest : public vis::ConfigurationUtils,
                               public ::testing::Test
{
    virtual void testing_method();

  protected:
};

void ConfigurationUtilsTest::testing_method()
{
}

TEST_F(ConfigurationUtilsTest, GetGradientIntervalSingle)
{
    EXPECT_EQ(255, get_gradient_interval(1, 256))
        << "gradient interval failed with single value";
}

TEST_F(ConfigurationUtilsTest, GetGradientIntervalTwo)
{
    EXPECT_EQ(255, get_gradient_interval(2, 256))
        << "gradient interval failed with single value";
}

TEST_F(ConfigurationUtilsTest, GetGradientIntervalZero)
{
    EXPECT_EQ(255, get_gradient_interval(0, 256)) << "gradient interval failed";
}
TEST_F(ConfigurationUtilsTest, GetGradientIntervalZeroNoColor)
{
    EXPECT_EQ(0, get_gradient_interval(0, 0)) << "gradient interval failed";
}
TEST_F(ConfigurationUtilsTest, GetGradientIntervalSingleNoColor)
{
    EXPECT_EQ(0, get_gradient_interval(1, 0)) << "gradient interval failed";
}

TEST_F(ConfigurationUtilsTest, GetGradientIntervalSameAsColor)
{
    EXPECT_EQ(0, get_gradient_interval(256, 256)) << "gradient interval failed";
}

TEST_F(ConfigurationUtilsTest, GetGradientIntervalBothOne)
{
    EXPECT_EQ(0, get_gradient_interval(1, 1)) << "gradient interval failed";
}

TEST_F(ConfigurationUtilsTest, GetGradientIntervalOneColor)
{
    EXPECT_EQ(0, get_gradient_interval(16, 1)) << "gradient interval failed";
}

TEST_F(ConfigurationUtilsTest, GetGradientIntervalNoColor)
{
    EXPECT_EQ(0, get_gradient_interval(16, 0)) << "gradient interval failed";
}

TEST_F(ConfigurationUtilsTest, GetGradientInterval32Color)
{
    EXPECT_EQ(8.5, get_gradient_interval(31, 256))
        << "gradient interval failed";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesSingleInvalidLine)
{
    std::vector<std::string> lines = {"hello world"};
    const auto colors = read_color_lines(true, lines);

    EXPECT_EQ(1, colors.size()) << "Should have read zero lines";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesSingleHexColor)
{
    std::vector<std::string> lines = {"#4040ff"};
    const auto colors = read_color_lines(true, lines);

    EXPECT_EQ(1, colors.size()) << "Incorrect number of colors read";

    EXPECT_EQ(1, colors[0].get_color_index()) << "incorrect color index";
    EXPECT_EQ(251, colors[0].get_red()) << "incorrect red color";
    EXPECT_EQ(251, colors[0].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[0].get_blue()) << "incorrect blue color";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesSingleHexColorNoOverride)
{
    std::vector<std::string> lines = {"#4040ff"};
    const auto colors = read_color_lines(false, lines);

    EXPECT_EQ(1, colors.size()) << "Incorrect number of colors read";

    EXPECT_EQ(64, colors[0].get_color_index()) << "incorrect color index";
    EXPECT_EQ(251, colors[0].get_red()) << "incorrect red color";
    EXPECT_EQ(251, colors[0].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[0].get_blue()) << "incorrect blue color";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesSingleSimpleColor)
{
    std::vector<std::string> lines = {"blue"};
    const auto colors = read_color_lines(false, lines);

    EXPECT_EQ(1, colors.size()) << "Incorrect number of colors read";

    EXPECT_EQ(4, colors[0].get_color_index()) << "incorrect color index";
    EXPECT_EQ(0x0, colors[0].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[0].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[0].get_blue()) << "incorrect blue color";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesSingleSimpleColorWithOverride)
{
    std::vector<std::string> lines = {"blue"};
    const auto colors = read_color_lines(true, lines);

    EXPECT_EQ(1, colors.size()) << "Incorrect number of colors read";

    EXPECT_EQ(1, colors[0].get_color_index()) << "incorrect color index";
    EXPECT_EQ(0x0, colors[0].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[0].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[0].get_blue()) << "incorrect blue color";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesMultiSimpleColor)
{
    std::vector<std::string> lines = {"blue", "red", "blue", "black"};
    const auto colors = read_color_lines(false, lines);

    EXPECT_EQ(4, colors.size()) << "Incorrect number of colors read";

    EXPECT_EQ(4, colors[0].get_color_index()) << "incorrect color index";
    EXPECT_EQ(0x0, colors[0].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[0].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[0].get_blue()) << "incorrect blue color";

    EXPECT_EQ(1, colors[1].get_color_index()) << "incorrect color index";
    EXPECT_EQ(1000, colors[1].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[1].get_green()) << "incorrect green color";
    EXPECT_EQ(0x0, colors[1].get_blue()) << "incorrect blue color";

    EXPECT_EQ(4, colors[2].get_color_index()) << "incorrect color index";
    EXPECT_EQ(0x0, colors[2].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[2].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[2].get_blue()) << "incorrect blue color";

    EXPECT_EQ(0, colors[3].get_color_index()) << "incorrect color index";
    EXPECT_EQ(0x0, colors[3].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[3].get_green()) << "incorrect green color";
    EXPECT_EQ(0x0, colors[3].get_blue()) << "incorrect blue color";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesMultiMixColor)
{
    std::vector<std::string> lines = {"blue", "#FF0033", "#4040FF", "black"};
    const auto colors = read_color_lines(true, lines);

    EXPECT_EQ(4, colors.size()) << "Incorrect number of colors read";

    EXPECT_EQ(1, colors[0].get_color_index()) << "incorrect color index";
    EXPECT_EQ(0x0, colors[0].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[0].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[0].get_blue()) << "incorrect blue color";

    EXPECT_EQ(2, colors[1].get_color_index()) << "incorrect color index";
    EXPECT_EQ(1000, colors[1].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[1].get_green()) << "incorrect green color";
    EXPECT_EQ(200, colors[1].get_blue()) << "incorrect blue color";

    EXPECT_EQ(3, colors[2].get_color_index()) << "incorrect color index";
    EXPECT_EQ(251, colors[2].get_red()) << "incorrect red color";
    EXPECT_EQ(251, colors[2].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[2].get_blue()) << "incorrect blue color";

    EXPECT_EQ(4, colors[3].get_color_index()) << "incorrect color index";
    EXPECT_EQ(0x0, colors[3].get_red()) << "incorrect red color";
    EXPECT_EQ(0x0, colors[3].get_green()) << "incorrect green color";
    EXPECT_EQ(0x0, colors[3].get_blue()) << "incorrect blue color";
}

TEST_F(ConfigurationUtilsTest, ReadColorLinesManyColors)
{
    std::vector<std::string> lines = {
        "#4040ff", "#2c56fc", "#2a59fc", "#1180ed", "#04a6d5", "#02abd1",
        "#03d2aa", "#04d6a5", "#12ee7f", "#2bfc58", "#2dfc55", "#56fc2d",
        "#59fc2a", "#80ed11", "#a6d503", "#abd103", "#d2aa03", "#d6a504",
        "#ee7f12", "#fc582b", "#fc552d", "#fc2c56", "#fc2a58", "#ed1180",
        "#d503a6", "#d102ab", "#aa03d2", "#a504d6", "#7f12ee", "#582bfc",
        "#552dfc"};

    const auto colors = read_color_lines(true, lines);

    EXPECT_EQ(31, colors.size()) << "Incorrect number of colors read";
}

TEST_F(ConfigurationUtilsTest, AddColorGradientsFirstColor)
{
    vis::ColorDefinition color1 = vis::ColorDefinition{1, 251, 251, 1000};

    std::vector<vis::ColorDefinition> colors;

    const auto gradient = get_gradient_interval(31, 256);
    add_color_gradients(true, color1, gradient, &colors);

    EXPECT_EQ(1, colors.size()) << "adding first gradient color";

    EXPECT_EQ(1, colors[0].get_color_index()) << "incorrect color index";
    EXPECT_EQ(251, colors[0].get_red()) << "incorrect red color";
    EXPECT_EQ(251, colors[0].get_green()) << "incorrect green color";
    EXPECT_EQ(1000, colors[0].get_blue()) << "incorrect blue color";
}

TEST_F(ConfigurationUtilsTest, AddColorGradientsManyHex)
{
    vis::ColorDefinition color1 = vis::ColorDefinition{1, 0x40, 0x40, 0xff};
    vis::ColorDefinition color2 = vis::ColorDefinition{2, 0x2c, 0x56, 0xfc};

    std::vector<vis::ColorDefinition> colors;

    const auto gradient = get_gradient_interval(31, 256);
    add_color_gradients(true, color1, gradient, &colors);
    add_color_gradients(true, color2, gradient, &colors);

    EXPECT_EQ(color1, colors.front()) << "First color should always be set";
    EXPECT_EQ(color2, colors.back()) << "Second color should always be set";

    EXPECT_EQ(9, colors.size()) << "Adding color gradient";
}

TEST_F(ConfigurationUtilsTest, AddColorGradientsZeroGradient)
{
    vis::ColorDefinition color1 = vis::ColorDefinition{1, 0x40, 0x40, 0xff};
    vis::ColorDefinition color2 = vis::ColorDefinition{2, 0x2c, 0x56, 0xfc};

    std::vector<vis::ColorDefinition> colors;

    add_color_gradients(true, color1, 0, &colors);
    add_color_gradients(true, color2, 0, &colors);

    EXPECT_EQ(color1, colors.front()) << "First color should always be set";
    EXPECT_EQ(color2, colors.back()) << "Second color should always be set";

    EXPECT_EQ(2, colors.size()) << "Adding color gradient";
}

TEST_F(ConfigurationUtilsTest, AddColorGradientsTwoHex)
{
    vis::ColorDefinition color1 = vis::ColorDefinition{1, 0x00, 0x00, 0x00};
    vis::ColorDefinition color2 = vis::ColorDefinition{2, 0xff, 0xff, 0xff};

    std::vector<vis::ColorDefinition> colors;

    const auto gradient = get_gradient_interval(2, 256);
    add_color_gradients(true, color1, gradient, &colors);
    add_color_gradients(true, color2, gradient, &colors);

    EXPECT_EQ(color1, colors.front()) << "First color should always be set";
    EXPECT_EQ(color2, colors.back()) << "Second color should always be set";

    EXPECT_EQ(255, gradient) << "Gradient should be across whole range";
    int16_t prev = -1;
    int32_t count = 0;
    for (const auto &color : colors)
    {
        if ((color.get_red() - prev) > 1)
        {
            count += 1;
        }
        prev = color.get_red();
    }
    EXPECT_EQ(1, count) << "colors are not 1 away";

    EXPECT_EQ(255, colors.size()) << "Expected to use the whole color range";
}

TEST_F(ConfigurationUtilsTest, AddColorGradientsTwoHexDedupe)
{
    vis::ColorDefinition color1 = vis::ColorDefinition{1, 0x40, 0x40, 0xff};
    vis::ColorDefinition color2 = vis::ColorDefinition{2, 0x2c, 0x56, 0xfc};

    std::vector<vis::ColorDefinition> colors;

    const auto gradient = get_gradient_interval(2, 256);
    add_color_gradients(true, color1, gradient, &colors);
    add_color_gradients(true, color2, gradient, &colors);

    EXPECT_EQ(color1, colors.front()) << "First color should always be set";
    EXPECT_EQ(color2, colors.back()) << "Second color should always be set";

    EXPECT_EQ(45, colors.size()) << "Expected to dedupe duplicate colors";
}

// TODO: test rainbow color generation

} // namespace vis
