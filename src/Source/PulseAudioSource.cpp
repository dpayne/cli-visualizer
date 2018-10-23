/*
 * PulseAudioSource.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Source/PulseAudioSource.h"
#include "Utils/Logger.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _LINUX
#include <unistd.h>
#endif

namespace
{
#ifdef _ENABLE_PULSE
static const char k_record_stream_name[] = "vis";
static const char k_record_stream_description[] = "vis stream";

static const int32_t k_sample_rate = 44100;
static const int32_t k_channels = 2;

static const std::string k_default_monitor_postfix = ".monitor";
#endif
} // namespace

vis::PulseAudioSource::PulseAudioSource(
    const std::shared_ptr<const vis::Settings> settings)
#ifdef _ENABLE_PULSE
    : m_settings{settings}, m_pulseaudio_simple{nullptr}, m_pulseaudio_mainloop{
                                                              nullptr}
{
}
#else
{
    // needed to make the compiler happy
    settings->get_color_schemes();
}
#endif

#ifdef _ENABLE_PULSE
void vis::PulseAudioSource::pulseaudio_server_info_callback(
    pa_context *, const pa_server_info *i, void *userdata)
{
    if (i != nullptr)
    {
        PulseAudioSource *vis_pa_src =
            reinterpret_cast<PulseAudioSource *>(userdata);
        std::string name = i->default_sink_name;
        name.append(k_default_monitor_postfix);

        vis_pa_src->m_pulseaudio_default_source_name = name;

        // stop mainloop after finding default name
        pa_mainloop_quit(vis_pa_src->m_pulseaudio_mainloop, 0);
    }
}

void vis::PulseAudioSource::pulseaudio_context_state_callback(pa_context *c,
                                                              void *userdata)
{

    switch (pa_context_get_state(c))
    {
    case PA_CONTEXT_UNCONNECTED:
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY:
    {
        pa_operation_unref(pa_context_get_server_info(
            c, pulseaudio_server_info_callback, userdata));
        break;
    }

    case PA_CONTEXT_FAILED:
    case PA_CONTEXT_TERMINATED:
        PulseAudioSource *vis_pa_src =
            reinterpret_cast<PulseAudioSource *>(userdata);
        pa_mainloop_quit(vis_pa_src->m_pulseaudio_mainloop, 0);
        break;
    }
}

#endif

void vis::PulseAudioSource::populate_default_source_name()
{
#ifdef _ENABLE_PULSE
    pa_mainloop_api *mainloop_api;
    pa_context *pulseaudio_context;

    // Create a mainloop API and connection to the default server
    m_pulseaudio_mainloop = pa_mainloop_new();

    mainloop_api = pa_mainloop_get_api(m_pulseaudio_mainloop);
    pulseaudio_context = pa_context_new(mainloop_api, "vis device list");

    // This function connects to the pulse server
    pa_context_connect(pulseaudio_context, nullptr, PA_CONTEXT_NOFLAGS,
                       nullptr);

    // This function defines a callback so the server will tell us its state.
    pa_context_set_state_callback(pulseaudio_context,
                                  pulseaudio_context_state_callback,
                                  reinterpret_cast<void *>(this));

    int ret;
    if (pa_mainloop_run(m_pulseaudio_mainloop, &ret) < 0)
    {
        VIS_LOG(vis::LogLevel::ERROR,
                "Could not open pulseaudio mainloop to "
                "find default device name: %d",
                ret);
    }
#endif
}

bool vis::PulseAudioSource::open_pulseaudio_source(
    const uint32_t max_buffer_size)
{
#ifdef _ENABLE_PULSE
    int32_t error_code = 0;

    static const pa_sample_spec sample_spec = {PA_SAMPLE_S16LE, k_sample_rate,
                                               k_channels};

    static const pa_buffer_attr buffer_attr = {max_buffer_size, 0, 0, 0,
                                               (max_buffer_size / 2)};

    auto audio_device = m_settings->get_pulse_audio_source();

    if (audio_device.empty())
    {
        populate_default_source_name();

        if (!m_pulseaudio_default_source_name.empty())
        {
            m_pulseaudio_simple =
                pa_simple_new(nullptr, k_record_stream_name, PA_STREAM_RECORD,
                              m_pulseaudio_default_source_name.c_str(),
                              k_record_stream_description, &sample_spec,
                              nullptr, &buffer_attr, &error_code);
        }

        // Try with the passing in nullptr, so that it will use the default
        // device
        if (m_pulseaudio_simple == nullptr)
        {
            m_pulseaudio_simple =
                pa_simple_new(nullptr, k_record_stream_name, PA_STREAM_RECORD,
                              nullptr, k_record_stream_description,
                              &sample_spec, nullptr, &buffer_attr, &error_code);
        }

        // if using default still did not work, try again with a common device
        // name
        if (m_pulseaudio_simple == nullptr)
        {
            m_pulseaudio_simple =
                pa_simple_new(nullptr, k_record_stream_name, PA_STREAM_RECORD,
                              "0", k_record_stream_description, &sample_spec,
                              nullptr, &buffer_attr, &error_code);
        }
    }
    else
    {
        m_pulseaudio_simple =
            pa_simple_new(nullptr, k_record_stream_name, PA_STREAM_RECORD,
                          audio_device.c_str(), k_record_stream_description,
                          &sample_spec, nullptr, &buffer_attr, &error_code);
    }

    if (m_pulseaudio_simple != nullptr)
    {
        return true;
    }

    VIS_LOG(vis::LogLevel::ERROR, "Could not open pulseaudio source %s: %s",
            audio_device.c_str(), pa_strerror(error_code));

    return false;
#else
    // needed to make the compiler happy
    return max_buffer_size == 0;
#endif
}

bool vis::PulseAudioSource::read(pcm_stereo_sample *buffer,
                                 const uint32_t buffer_size)
{
    auto buffer_size_bytes =
        static_cast<size_t>(sizeof(pcm_stereo_sample) * buffer_size);

#ifdef _ENABLE_PULSE

    if (m_pulseaudio_simple == nullptr)
    {
        open_pulseaudio_source(static_cast<uint32_t>(buffer_size_bytes));
    }

    if (m_pulseaudio_simple != nullptr)
    {
        // zero out buffer
        memset(buffer, 0, buffer_size_bytes);

        int32_t error_code;
        /* Record some data ... */
        auto return_code = pa_simple_read(m_pulseaudio_simple, buffer,
                                          buffer_size_bytes, &error_code);

        if (return_code < 0)
        {
            VIS_LOG(vis::LogLevel::WARN,
                    "Could not finish reading pulse audio "
                    "stream buffer, bytes read: %d buffer "
                    "size: ",
                    return_code, buffer_size_bytes);

            // zero out buffer
            memset(buffer, 0, buffer_size_bytes);

            pa_simple_free(m_pulseaudio_simple);
            m_pulseaudio_simple = nullptr;

            return false;
        }

        // Success fully read entire buffer
        return true;
    }
#endif
    // zero out buffer
    memset(buffer, 0, buffer_size_bytes);

    return false;
}

#ifdef _ENABLE_PULSE
vis::PulseAudioSource::~PulseAudioSource()
{
    if (m_pulseaudio_simple != nullptr)
    {
        pa_simple_free(m_pulseaudio_simple);
    }
}
#endif
