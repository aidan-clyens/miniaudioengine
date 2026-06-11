#include "audiosession.h"

#include "trackservice.h"
#include "deviceservice.h"
#include "fileservice.h"

#include "audioadapter.h"
#include "midiadapter.h"
#include "fileadapter.h"

#include <unistd.h>
#include <fcntl.h>

using namespace miniaudioengine;
using namespace miniaudioengine::adapters;

/** @brief Temporarily suppress stderr output during MIDI initialization.
 *  Redirects stderr to /dev/null to prevent ALSA library errors from printing
 *  when MIDI hardware is unavailable (e.g., in Docker containers).
 */
class StderrRedirector
{
public:
  StderrRedirector()
    : m_original_stderr_fd(dup(STDERR_FILENO))
  {
    // Redirect stderr to /dev/null
    int dev_null = open("/dev/null", O_WRONLY);
    if (dev_null >= 0)
    {
      dup2(dev_null, STDERR_FILENO);
      close(dev_null);
    }
  }

  ~StderrRedirector()
  {
    // Restore original stderr
    if (m_original_stderr_fd >= 0)
    {
      dup2(m_original_stderr_fd, STDERR_FILENO);
      close(m_original_stderr_fd);
    }
  }

private:
  int m_original_stderr_fd;
};

AudioSession::AudioSession()
{
  p_audio_adapter = std::make_shared<AudioAdapter>();
  
  // Attempt MIDI adapter initialization, but don't fail if MIDI is unavailable
  // Suppress stderr to prevent ALSA library errors from printing
  {
    StderrRedirector suppress_alsa_errors;
    try
    {
      p_midi_adapter = std::make_shared<MidiAdapter>();
    }
    catch (const std::runtime_error& error)
    {
      LOG_WARNING("AudioSession: MIDI initialization failed - ", error.what(), ". Audio-only mode enabled.");
      p_midi_adapter = nullptr;
    }
  }
  
  p_file_adapter = std::make_shared<FileAdapter>();

  p_file_service = std::make_unique<FileService>();
  p_device_service = std::make_unique<DeviceService>(p_audio_adapter, p_midi_adapter);
  p_track_service = std::make_unique<TrackService>(p_audio_adapter, p_midi_adapter);

  LOG_INFO("AudioSession: Initialized!");
}

AudioSession::~AudioSession() = default;

DeviceList AudioSession::get_audio_devices() const
{
  return p_device_service->get_audio_devices();
}

DeviceList AudioSession::get_midi_devices() const
{
  return p_device_service->get_midi_devices();
}

DevicePtr AudioSession::get_audio_device(unsigned int device_id) const
{
  return p_device_service->get_audio_device(device_id);
}

DevicePtr AudioSession::get_midi_device(unsigned int device_id) const
{
  return p_device_service->get_midi_device(device_id);
}

DevicePtr AudioSession::get_default_audio_input_device() const
{
  return p_device_service->get_default_audio_input_device();
}

DevicePtr AudioSession::get_default_audio_output_device() const
{
  return p_device_service->get_default_audio_output_device();
}

FileList AudioSession::get_audio_files(const std::filesystem::path &directory) const
{
  return p_file_service->get_audio_files(directory);
}

FileList AudioSession::get_midi_files(const std::filesystem::path &directory) const
{
  return p_file_service->get_midi_files(directory);
}

FilePtr AudioSession::get_audio_file(const std::filesystem::path &file_path) const
{
  return p_file_service->get_audio_file(file_path);
}

FilePtr AudioSession::get_midi_file(const std::filesystem::path &file_path) const
{
  return p_file_service->get_midi_file(file_path);
}

TrackList AudioSession::get_tracks() const
{
  return p_track_service->get_all_tracks();
}

TrackPtr AudioSession::add_track() const
{
  return p_track_service->add_track();
}

bool AudioSession::play()
{
  bool ret = p_track_service->get_main_track()->play();
  m_state = ret ? eAudioSessionState::Playing : eAudioSessionState::Stopped;
  return ret;
}

bool AudioSession::record()
{
  // TODO - Implement
  LOG_ERROR("Record functionality not implemented yet.");
  return false;
}

bool AudioSession::stop()
{
  bool ret = p_track_service->get_main_track()->stop();
  m_state = ret ? eAudioSessionState::Stopped : eAudioSessionState::Playing;
  return ret;
}
