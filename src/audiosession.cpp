#include "audiosession.h"

#include "trackservice.h"
#include "deviceservice.h"
#include "fileservice.h"

#include "audioadapter.h"
#include "midiadapter.h"
#include "fileadapter.h"

using namespace miniaudioengine;
using namespace miniaudioengine::adapters;

AudioSession::AudioSession()
{
  p_audio_adapter = std::make_shared<AudioAdapter>();
  p_midi_adapter = std::make_shared<MidiAdapter>();
  p_file_adapter = std::make_shared<FileAdapter>();

  p_file_service = std::make_unique<FileService>();
  p_device_service = std::make_unique<DeviceService>(p_audio_adapter, p_midi_adapter);
  p_track_service = std::make_unique<TrackService>(p_audio_adapter, p_midi_adapter);
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
  throw new std::exception("Record functionality not implemented yet.");
}

bool AudioSession::stop()
{
  bool ret = p_track_service->get_main_track()->stop();
  m_state = ret ? eAudioSessionState::Stopped : eAudioSessionState::Playing;
  return ret;
}
