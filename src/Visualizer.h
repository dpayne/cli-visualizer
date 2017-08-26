/*
 * Visualizer.h
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#ifndef _VIS_VISUALIZER_H
#define _VIS_VISUALIZER_H

#include "Domain/Settings.h"
#include "Domain/VisTypes.h"
#include "Source/AudioSource.h"
#include "Transformer/GenericTransformer.h"
#include "Writer/NcursesWriter.h"
#include <locale>
#include <memory>

namespace vis
{

class Visualizer
{
  public:
    explicit Visualizer(const std::string &config_path, const std::locale &loc);

    Visualizer(const Visualizer &v) = delete;

    Visualizer(const Visualizer &&v) = delete;

    Visualizer &operator=(const Visualizer &v) = delete;

    Visualizer &operator=(const Visualizer &&v) = delete;

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

    void reload_config();

  private:
    std::shared_ptr<vis::Settings> m_settings;

    size_t m_current_color_scheme_index;

    size_t m_current_transformer_index;

    std::unique_ptr<vis::NcursesWriter> m_writer;

    std::unique_ptr<AudioSource> m_audio_source;

    bool m_shutdown;

    bool m_signal_handlers_setup;

    std::locale m_loc;

    std::vector<std::unique_ptr<vis::GenericTransformer>> m_transformers;

    pcm_stereo_sample *m_pcm_buffer;

    inline GenericTransformer *get_current_transformer()
    {
        return m_transformers[m_current_transformer_index].get();
    }

    void process_user_input();

    void rotate_color_scheme();

    void setup_audio_source();

    void setup_transformers();

    void setup_signal_handlers();

    void rotate_transformer(int64_t rotation_interval,
                            int64_t *last_rotation_timestamp);

    void allocate_buffer();
};
} // namespace vis

#endif
