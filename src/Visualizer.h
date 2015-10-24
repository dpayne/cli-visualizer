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
#include "Domain/VisTypes.h"
#include "Source/AudioSource.h"
#include "Transformer/GenericTransformer.h"

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

    GenericTransformer *m_current_transformer;

    bool m_shutdown;

    const vis::Settings *const m_settings;

    std::vector<std::unique_ptr<vis::AudioSource>> m_audio_sources;

    std::vector<std::unique_ptr<vis::GenericTransformer>> m_transformers;

    void add_audio_source(const std::string &audio_source);

    static const int32_t k_pcm_buffer_size = 1024;
    pcm_stereo_sample m_pcm_buffer[k_pcm_buffer_size];

    inline AudioSource *get_current_audio_source()
    {
        return m_current_audio_source;
    }

    inline GenericTransformer *get_current_transformer()
    {
        return m_current_transformer;
    }

    void setup_audio_sources();

    void setup_transformers();
};
}

#endif
