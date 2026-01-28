#ifndef __AUDIO_CONTROLLER_MOCK_H__
#define __AUDIO_CONTROLLER_MOCK_H__

#include <optional>
#include <thread>
#include <chrono>
#include <memory>

#include "audiocontroller.h"
#include "audiocallbackhandler.h"
#include "trackmanager.h"
#include "device.h"
#include "logger.h"

using namespace miniaudioengine::core;
using namespace miniaudioengine::control;
using namespace miniaudioengine::data;

namespace miniaudioengine::test
{

enum RtAudioErrorType
{
  RTAUDIO_NO_ERROR = 0,
  RTAUDIO_WARNING,
  RTAUDIO_ERROR,
};

class AudioControllerMock : public IAudioController
{
public:
  explicit AudioControllerMock()
  {
    m_audio_output_device.id = 0;
    m_audio_output_device.name = "Mock Output Device";
    m_audio_output_device.is_default_output = true;
    m_audio_output_device.is_default_input = false;
    m_audio_output_device.output_channels = 2;
    m_audio_output_device.input_channels = 0;
    m_audio_output_device.duplex_channels = 0;
    m_audio_output_device.sample_rates = {44100, 48000};
    m_audio_output_device.preferred_sample_rate = 44100;

    m_audio_input_device.id = 1;
    m_audio_input_device.name = "Mock Input Device";
    m_audio_input_device.is_default_output = false;
    m_audio_input_device.is_default_input = true;
    m_audio_input_device.output_channels = 0;
    m_audio_input_device.input_channels = 2;
    m_audio_input_device.duplex_channels = 0;
    m_audio_input_device.sample_rates = {44100, 48000};
    m_audio_input_device.preferred_sample_rate = 44100;
  }

  ~AudioControllerMock() = default;

  std::vector<AudioDevice> get_audio_devices() override
  {
    return {
      m_audio_output_device,
      m_audio_input_device
    };
  }

  AudioDevice get_audio_output_device_mock() const
  {
    return m_audio_output_device;
  }

  AudioDevice get_audio_input_device_mock() const
  {
    return m_audio_input_device;
  }

  bool start_stream() override
  {
    // Use base class validation
    if (!validate_start_preconditions())
    {
      return false;
    }

    if (!register_dataplanes())
    {
      return false;
    }

    // Simulate successful stream start
    LOG_INFO("AudioControllerMock: RtAudio stream started successfully.");
    m_stream_state = eStreamState::Playing;
    m_is_stream_running = true;

    return true;
  }

  bool stop_stream() override
  {
    if (m_stream_state != eStreamState::Playing)
    {
      LOG_WARNING("AudioControllerMock: Stream is not running. No action taken.");
      return false;
    }

    LOG_INFO("AudioControllerMock: RtAudio stream stopped successfully.");
    m_stream_state = eStreamState::Stopped;
    m_is_stream_running = false;

    return true;
  }

private:
  bool m_is_stream_running{false};

  AudioDevice m_audio_output_device;
  AudioDevice m_audio_input_device;
};

using AudioControllerMockPtr = std::shared_ptr<AudioControllerMock>;

} // namespace miniaudioengine::test

#endif // __AUDIO_CONTROLLER_MOCK_H__