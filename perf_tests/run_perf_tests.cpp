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

static void test_color_settings(benchmark::State &state)
{
    while (state.KeepRunning())
    {
    }
}

BENCHMARK(test_color_settings);
BENCHMARK_MAIN();
