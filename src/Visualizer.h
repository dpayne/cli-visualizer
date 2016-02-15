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
#include "Writer/NcursesWriter.h"

namespace vis
{

class Visualizer
{
  public:
    explicit Visualizer(vis::Settings *settings);

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

    void process_user_input();

    void reload_config();

  private:
    size_t m_current_audio_source_index;

    size_t m_current_transformer_index;

    std::unique_ptr<vis::NcursesWriter> m_writer;

    bool m_shutdown;

    bool m_signal_handlers_setup;

    vis::Settings *m_settings;

    std::vector<std::unique_ptr<vis::AudioSource>> m_audio_sources;

    std::vector<std::unique_ptr<vis::GenericTransformer>> m_transformers;

    pcm_stereo_sample *m_pcm_buffer;

    void add_audio_source(const std::string &audio_source);

    inline AudioSource *get_current_audio_source()
    {
        return m_audio_sources[m_current_audio_source_index].get();
    }

    inline GenericTransformer *get_current_transformer()
    {
        return m_transformers[m_current_transformer_index].get();
    }

    void setup_audio_sources();

    void setup_transformers();

    void setup_signal_handlers();

    void rotate_transformer(const int64_t rotation_interval,
                            int64_t *last_rotation_timestamp);
};
}

#endif
