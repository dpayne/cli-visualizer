/*
 * Visualizer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_VISUALIZER_H
#define _VIS_VISUALIZER_H

#include <memory>
#include "Domain/Settings.h"
#include "Source/AudioSource.h"

namespace vis
{

class Visualizer
{
  public:
    explicit Visualizer(const vis::Settings *const settings);

    virtual ~Visualizer();

    void run();

    inline bool should_shutdown() const
    {
        return m_shutdown;
    }

    inline void shutdown()
    {
        m_shutdown = true;
    }

  private:
    AudioSource *m_current_audio_source;

    bool m_shutdown;

    const vis::Settings *const m_settings;

    std::vector<std::unique_ptr<vis::AudioSource>> m_audio_sources;

    void add_audio_source(const std::string &audio_source);

    static const int32_t k_pcm_buffer_size = 1024;
    char m_pcm_buffer[k_pcm_buffer_size];

    AudioSource *get_current_audio_source()
    {
        return m_current_audio_source;
    };
};
}

#endif
