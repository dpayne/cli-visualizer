/*
 * vis.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <csignal>
#include <cstring>
#include <iostream>
#include <memory>

#include "Domain/Settings.h"
#include "Domain/VisConstants.h"
#include "Domain/VisException.h"
#include "Utils/ConfigurationUtils.h"
#include "Utils/Logger.h"
#include "Utils/NcursesUtils.h"
#include "Utils/Utils.h"
#include "Visualizer.h"

#ifdef NCURSESW
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

static std::string g_program_help =
    "Usage: vis -c FILE\n"
    "vis -- CLI visualizer.\n"
    "\n"
    "  -c FILE     Config file path, defaults to ~/.config/vis/config\n"
    "  -h          Give this help list\n";

int main(int argc, char *argv[])
{
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

    std::locale loc; // initialized to locale::classic()
    std::ios::sync_with_stdio(false);

    try
    {
        loc = std::locale{VisConstants::k_default_locale.c_str()};
        std::wcout.imbue(loc); // Use it for output
    }
    catch (const std::runtime_error &)
    {
        loc = std::locale{loc, "", std::locale::ctype};
    }

    std::string error_msg;
    try
    {
        // use default config path if none given
        if (config_path.empty())
        {
            config_path = VisConstants::k_default_config_path;
        }

        std::unique_ptr<vis::Visualizer> visualizer =
            std::make_unique<vis::Visualizer>(config_path, loc);

        visualizer->run();
    }
    catch (const vis::VisException &ex)
    {
        VIS_LOG(vis::LogLevel::ERROR, "vis exception: %s", ex.what());
        error_msg.append(ex.what());
    }
    catch (const std::exception &ex)
    {
        VIS_LOG(vis::LogLevel::ERROR, "standard exception: %s", ex.what());
        error_msg.append(ex.what());
    }
    catch (...)
    {
        VIS_LOG(vis::LogLevel::ERROR, "unknown exception");
        error_msg.append("unknown exception");
    }

    vis::Logger::uninitialize();

    if (!error_msg.empty())
    {
        std::cout << error_msg << std::endl;
    }
}
