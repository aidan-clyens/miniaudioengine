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

  p_device_service = std::make_unique<DeviceService>(p_audio_adapter, p_midi_adapter);
  p_file_service = std::make_unique<FileService>();
  p_track_service = std::make_unique<TrackService>();
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
  // TODO - Implement
  return std::vector<TrackPtr>();
}

TrackPtr AudioSession::add_track() const
{
  // TODO - Implement
  return nullptr;
}

bool AudioSession::play()
{
  // TODO - Implement
  return false;
}

bool AudioSession::record()
{
  // TODO - Implement
  return false;
}

bool AudioSession::stop()
{
  // TODO - Implement
  return false;
}
