#ifndef __AUDIO_CONTROLLER_MOCK_H__
#define __AUDIO_CONTROLLER_MOCK_H__

#include <optional>
#include <thread>
#include <chrono>
#include <memory>

#include "audiocontroller.h"
#include "trackservice.h"
#include "logger.h"

using namespace miniaudioengine::framework;
using namespace miniaudioengine;
using namespace miniaudioengine::audio;

namespace miniaudioengine::test
{

enum RtAudioErrorType
{
  RTAUDIO_NO_ERROR = 0,
  RTAUDIO_WARNING,
  RTAUDIO_ERROR,
};

class MockAudioController : public AudioController
{
public:
  explicit MockAudioController()
  {
    p_audio_output_device = DeviceHandleFactory::make_audio(
      0, "Mock Output Device",
      /*is_default_input=*/false, /*is_default_output=*/true,
      /*output_channels=*/2, /*input_channels=*/0, /*duplex=*/0,
      /*preferred_sample_rate=*/44100, {44100, 48000});

    p_audio_input_device = DeviceHandleFactory::make_audio(
      1, "Mock Input Device",
      /*is_default_input=*/true, /*is_default_output=*/false,
      /*output_channels=*/0, /*input_channels=*/2, /*duplex=*/0,
      /*preferred_sample_rate=*/44100, {44100, 48000});
  }

  ~MockAudioController() = default;

  std::vector<DeviceHandlePtr> get_audio_devices() override
  {
    return { p_audio_output_device, p_audio_input_device };
  }

  DeviceHandlePtr get_audio_output_device_mock() const
  {
    return p_audio_output_device;
  }

  DeviceHandlePtr get_audio_input_device_mock() const
  {
    return p_audio_input_device;
  }

private:
  bool m_is_stream_running{false};

  DeviceHandlePtr p_audio_output_device;
  DeviceHandlePtr p_audio_input_device;

  bool _start() override
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
    LOG_INFO("MockAudioController: RtAudio stream started successfully.");
    m_stream_state = eStreamState::Playing;
    m_is_stream_running = true;

    return true;
  }

  bool _stop() override
  {
    if (m_stream_state != eStreamState::Playing)
    {
      LOG_WARNING("MockAudioController: Stream is not running. No action taken.");
      return false;
    }

    LOG_INFO("MockAudioController: RtAudio stream stopped successfully.");
    m_stream_state = eStreamState::Stopped;
    m_is_stream_running = false;

    return true;
  }
};

using MockAudioControllerPtr = std::shared_ptr<MockAudioController>;

} // namespace miniaudioengine::test

#endif // __AUDIO_CONTROLLER_MOCK_H__