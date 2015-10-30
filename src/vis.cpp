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
#include "Visualizer.h"

static vis::Visualizer *g_vis = nullptr;

static inline void shutdown(int sig)
{
    std::cout << "Received signal: " << sig << std::endl;

    if (g_vis != nullptr)
    {
        g_vis->shutdown();
    }
}

int main()
{

    std::signal(SIGINT, shutdown);
    std::signal(SIGTERM, shutdown);

    vis::Settings settings;
    settings.set_mpd_fifo_path("/tmp/mpd.fifo");
    settings.set_audio_sources({"mpd"});

    std::unique_ptr<vis::Visualizer> visualizer{new vis::Visualizer{&settings}};
    g_vis = visualizer.get();

    visualizer->run();
}
