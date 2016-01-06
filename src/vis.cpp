/*
 * vis.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <iostream>
#include <memory>
#include <csignal>
#include <string.h>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"
#include "Utils/ConfigurationUtils.h"
#include "Utils/Utils.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Visualizer.h"

#ifdef NCURSESW
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

static vis::Visualizer *g_vis = nullptr;
static std::string g_program_help =
    "Usage: vis -c FILE\n"
    "vis -- CLI visualizer.\n"
    "\n"
    "  -c FILE     Config file path, defaults to ~/.config/vis/config\n"
    "  -h          Give this help list\n";

static inline void shutdown(int sig)
{
    std::cerr << "Received signal: " << sig << std::endl;

    if (g_vis != nullptr)
    {
        g_vis->shutdown();
    }
}

int main(int argc, char *argv[])
{
    // Catch interrupt and termination signals so the program can be cleanly
    // shutdown.
    std::signal(SIGINT, shutdown);
    std::signal(SIGTERM, shutdown);

    std::string config_path;

    // Read the settings file command line argument if available
    if (argc > 1)
    {
        for (auto i = 1; i < argc; ++i)
        {
            char *arg = argv[i];
            if (strcmp(arg, "-c") == 0 && (i + 1) < argc)
            {
                config_path = std::string{argv[i + 1]};
            }
            else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
            {
                std::cout << g_program_help << std::endl;
                return 0;
            }
            std::cout << arg << std::endl;
        }
    }

    vis::Logger::initialize(VisConstants::k_default_log_path);

    vis::Settings settings;

    // use default config path if none given
    if (config_path.empty())
    {
        vis::ConfigurationUtils::load_settings(settings);
    }
    else
    {
        vis::ConfigurationUtils::load_settings(settings, config_path);
    }

    std::unique_ptr<vis::Visualizer> visualizer =
        std::make_unique<vis::Visualizer>(&settings);
    g_vis = visualizer.get();

    visualizer->run();

    vis::Logger::uninitialize();

    // Clears the terminal on exit
    system("setterm -blank 10");
    system("clear");
}
