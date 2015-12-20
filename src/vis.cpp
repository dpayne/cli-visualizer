/*
 * vis.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <iostream>
#include <memory>
#include <csignal>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"
#include "Utils/ConfigurationUtils.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Visualizer.h"

#include <ncurses.h>

static vis::Visualizer *g_vis = nullptr;

static inline void shutdown(int sig)
{
    std::cerr << "Received signal: " << sig << std::endl;

    if (g_vis != nullptr)
    {
        g_vis->shutdown();
    }
}

int main()
{
    std::signal(SIGINT, shutdown);
    std::signal(SIGTERM, shutdown);

    vis::Logger::initialize(VisConstants::k_default_log_path);

    vis::Settings settings;

    vis::ConfigurationUtils::load_settings(settings);

    std::unique_ptr<vis::Visualizer> visualizer = std::make_unique<vis::Visualizer>(&settings);
    g_vis = visualizer.get();

    visualizer->run();

    vis::Logger::uninitialize();
    system("setterm -blank 10");
    system("clear");
}
