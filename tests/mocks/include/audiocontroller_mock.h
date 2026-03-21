#ifndef __AUDIO_CONTROLLER_MOCK_H__
#define __AUDIO_CONTROLLER_MOCK_H__

#include <optional>
#include <thread>
#include <chrono>
#include <memory>

#include "audiocontroller.h"
#include "audiocallbackhandler.h"
#include "miniaudioengine/trackmanager.h"
#include "device.h"
#include "logger.h"

#include "device_mock.h"

using namespace miniaudioengine::core;
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

class MockAudioController : public IAudioController
{
public:
  explicit MockAudioController()
  {
    p_audio_output_device = std::make_shared<MockAudioOutputDevice>();
    p_audio_input_device = std::make_shared<MockAudioOutputDevice>();

    auto audio_output = std::dynamic_pointer_cast<MockAudioOutputDevice>(p_audio_output_device);
    auto audio_input = std::dynamic_pointer_cast<MockAudioOutputDevice>(p_audio_input_device);

    audio_output->id = 0;
    audio_output->name = "Mock Output Device";
    audio_output->is_default_output = true;
    audio_output->is_default_input = false;
    audio_output->output_channels = 2;
    audio_output->input_channels = 0;
    audio_output->duplex_channels = 0;
    audio_output->sample_rates = {44100, 48000};
    audio_output->preferred_sample_rate = 44100;

    audio_input->id = 1;
    audio_input->name = "Mock Input Device";
    audio_input->is_default_output = false;
    audio_input->is_default_input = true;
    audio_input->output_channels = 0;
    audio_input->input_channels = 2;
    audio_input->duplex_channels = 0;
    audio_input->sample_rates = {44100, 48000};
    audio_input->preferred_sample_rate = 44100;
  }

  ~MockAudioController() = default;

  std::vector<core::IAudioDevicePtr> get_audio_devices() override
  {
    return
    {
      p_audio_output_device,
      p_audio_input_device
    };
  }

  MockAudioOutputDevicePtr get_audio_output_device_mock() const
  {
    return std::dynamic_pointer_cast<MockAudioOutputDevice>(p_audio_output_device);
  }

  MockAudioOutputDevicePtr get_audio_input_device_mock() const
  {
    return std::dynamic_pointer_cast<MockAudioOutputDevice>(p_audio_input_device);
  }

private:
  bool m_is_stream_running{false};

  MockAudioOutputDevicePtr p_audio_output_device;
  MockAudioOutputDevicePtr p_audio_input_device;

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