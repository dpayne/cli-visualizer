/*
 * PortAudioSource.cpp
 *
 * Created on: May 1, 2020
 *     Author: noriah
 */

#include "Source/PortAudioSource.h"
#include "Domain/VisException.h"
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
#ifdef _ENABLE_PORT
static const int32_t k_channels = 2;

#endif
} // namespace

vis::PortAudioSource::PortAudioSource(
    const std::shared_ptr<const vis::Settings> settings)
#ifdef _ENABLE_PORT
    : m_settings{settings},
    m_portaudio_stream{nullptr}
{
    PaError err = paNoError;
    err = Pa_Initialize();
    if (err != paNoError) {
        VIS_LOG(vis::LogLevel::ERROR,
                "Could not initialize port audio. (%d)\n%s\n",
                err, Pa_GetErrorText( err ));
    }
}
#else
{
    // needed to make the compiler happy
    settings->get_color_schemes();
}
#endif

bool vis::PortAudioSource::open_portaudio_source(
    const uint32_t max_buffer_size)
{
#ifdef _ENABLE_PORT
    auto audio_device = m_settings->get_port_audio_source();

    int deviceNum = -1, numDevices = Pa_GetDeviceCount();

    if (audio_device.empty() || !strcmp(audio_device.c_str(), "auto"))
    {
        deviceNum = Pa_GetDefaultInputDevice();

        if (deviceNum == paNoDevice) {
            VIS_LOG(vis::LogLevel::ERROR, "found no default device");
            return false;
        }
    }
    else if (!strcmp(audio_device.c_str(), "list"))
    {
        if (numDevices < 0) {
            VIS_LOG(vis::LogLevel::ERROR,
            "No devices found with port audio");
            return false;
        }

        for (int i = 0; i < numDevices; i++) {
            const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
            VIS_LOG(vis::LogLevel::ERROR,
                    "Device #%d: %s\n"
                   "\tInput Channels: %d\n"
                   "\tOutput Channels: %d\n"
                   "\tDefault Sample Rate: %f",
                   i + 1, deviceInfo->name, deviceInfo->maxInputChannels,
                   deviceInfo->maxOutputChannels, deviceInfo->defaultSampleRate);
        }

        return false;
    }
    else if (sscanf(audio_device.c_str(), "%d", &deviceNum))
    {
        if (deviceNum > numDevices) {
            VIS_LOG(vis::LogLevel::ERROR, "bad device num: %d", deviceNum);
            return false;
        }
    }
    else
    {
        for (int i = 0; i < numDevices; i++) {
            const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
            if (!strcmp(audio_device.c_str(), deviceInfo->name)) {
                deviceNum = i;
                break;
            }
        }
        if (deviceNum == -1) {
            VIS_LOG(vis::LogLevel::ERROR, "bad device: %s", audio_device.c_str());
            return false;
        }
    }

    m_portaudio_params.device = deviceNum;
    m_portaudio_params.channelCount = k_channels;
    m_portaudio_params.sampleFormat = paInt16;
    m_portaudio_params.suggestedLatency =
        Pa_GetDeviceInfo(m_portaudio_params.device)->defaultLowInputLatency;
    m_portaudio_params.hostApiSpecificStreamInfo = NULL;


    PaError err = paNoError;
    err = Pa_OpenStream(&m_portaudio_stream, &m_portaudio_params, NULL,
                        m_settings->get_sampling_frequency(), max_buffer_size, paClipOff,
                        NULL, NULL);

    if (err == paNoError && m_portaudio_stream != nullptr)
    {
        err = Pa_StartStream( m_portaudio_stream );
        if (err != paNoError) {
            VIS_LOG(vis::LogLevel::ERROR, "Could not open portaudio source %s: %s",
            audio_device.c_str(), Pa_GetErrorText( err ));
            return false;
        }

        return true;
    }

    VIS_LOG(vis::LogLevel::ERROR, "Could not open portaudio source %s: %s",
            audio_device.c_str(), Pa_GetErrorText( err ));

    return false;
#else
    // needed to make the compiler happy
    return max_buffer_size == 0;
#endif
}

bool vis::PortAudioSource::read(pcm_stereo_sample *buffer,
                                 const uint32_t buffer_size)
{
    auto buffer_size_bytes =
        static_cast<size_t>(sizeof(pcm_stereo_sample) * buffer_size);

#ifdef _ENABLE_PORT

    if (m_portaudio_stream == nullptr)
    {
        if (!open_portaudio_source(buffer_size)) {
            throw vis::VisException{"Failed to open Port audio"};
        }
    }

    if (m_portaudio_stream != nullptr)
    {
        // zero out buffer
        memset(buffer, 0, buffer_size_bytes);

        /* Record some data ... */
        PaError err = Pa_ReadStream(m_portaudio_stream, buffer,
                                          buffer_size);

        if (err != paNoError)
        {
            VIS_LOG(vis::LogLevel::WARN,
                    "Could not finish reading port audio "
                    "stream buffer, bytes read: %d buffer "
                    "size: ",
                    err, buffer_size_bytes);

            // zero out buffer
            memset(buffer, 0, buffer_size_bytes);

            err = Pa_CloseStream(m_portaudio_stream);
            if (err != paNoError) {
                VIS_LOG(vis::LogLevel::ERROR,
                    "couldn't close stream",
                    err);
            }
            m_portaudio_stream = nullptr;

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

#ifdef _ENABLE_PORT
vis::PortAudioSource::~PortAudioSource()
{
    if (m_portaudio_stream != nullptr)
    {
        PaError err = paNoError;
        err = Pa_CloseStream(m_portaudio_stream);
        if (err != paNoError) {
            VIS_LOG(vis::LogLevel::ERROR,
                    "Could not close port audio stream. (%d)\n%s\n",
                    err, Pa_GetErrorText( err ));
        }
    }

    Pa_Terminate();
}
#endif
