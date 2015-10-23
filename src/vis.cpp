/*
 * vis.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <iostream>
#include <memory>
#include "Domain/Settings.h"
#include "Visualizer.h"

int main()
{
    vis::Settings settings;
    settings.set_mpd_fifo_path( "/tmp/mpd.fifo" );
    settings.set_audio_sources( { "mpd" } );

    std::unique_ptr<vis::Visualizer> visualizer{ new vis::Visualizer{ &settings } };
    visualizer->run();
}
