#include <benchmark/benchmark.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string.h>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"
#include "Utils/ConfigurationUtils.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Utils/Utils.h"
#include "Visualizer.h"
#include "Writer/NcursesWriter.h"

#include <ncurses.h>

static void test_color_settings(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        // place holder for any perf tests
    }
}

BENCHMARK(test_color_settings);
BENCHMARK_MAIN();
