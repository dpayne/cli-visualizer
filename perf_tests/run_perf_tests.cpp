#include <benchmark/benchmark.h>
#include <iostream>
#include <memory>
#include <csignal>
#include <string.h>
#include <cstdlib>
#include <numeric>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"
#include "Utils/ConfigurationUtils.h"
#include "Utils/Utils.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Writer/NcursesWriter.h"
#include "Visualizer.h"

#include <ncurses.h>

static const double k_pi = 3.14159265358979323846;

static void test_color_settings(benchmark::State &state)
{
    while (state.KeepRunning())
    {
    }
}

// BENCHMARK(test_color_settings);

// BENCHMARK_MAIN();
//
int16_t to_ansi_color_domain(int16_t color)
{
    return static_cast<int16_t>(6.0 * static_cast<double>(color) / 256.0);
}

int16_t to_ansi_color(int16_t red, int16_t green, int16_t blue)
{
    return 16 + static_cast<int16_t>((to_ansi_color_domain(red) * 36.0) +
                                     (to_ansi_color_domain(green) * 6.0) +
                                     (to_ansi_color_domain(blue) * 1.0));
}

int main()
{
    vis::Settings settings;
    vis::ConfigurationUtils::load_settings(settings);
    vis::NcursesWriter m_writer{&settings};

    auto freq = 0.1;
    std::wstring msg{L" "};

    for (auto i = 0; i < 64; ++i)
    {
        auto red = i;
        auto green = 0;
        auto blue = 0;

        m_writer.write(0, i, to_ansi_color(red, green, blue), msg);
    }

    for (auto i = 255; i >= 0; --i)
    {
        auto blue =
            static_cast<int16_t>(std::cos(freq * i + 0.0) * 127.0 + 128.0);
        auto red = static_cast<int16_t>(
            std::cos(freq * i + 2.0 * k_pi / 3.0) * 127 + 128);
        auto green = static_cast<int16_t>(
            std::cos(freq * i + 4.0 * k_pi / 3.0) * 127 + 128);

        m_writer.write_background(1, 255 - i, to_ansi_color(red, green, blue),
                                  msg);
    }

    for (auto i = 0; i < 256; ++i)
    {
        m_writer.write_background(2, i, m_writer.to_color_rainbow(i, 256), msg);
    }

    for (auto i = 0; i < 256; ++i)
    {
        m_writer.write_background(3, i, i, msg);
    }
    getch();
}
