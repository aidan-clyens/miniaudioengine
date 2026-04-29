#ifndef __AUDIO_SESSION_H__
#define __AUDIO_SESSION_H__

#include <vector>
#include <memory>
#include <filesystem>

namespace miniaudioengine
{

// Forward declarations
class Device;
class File;
class Track;

class TrackService;
class DeviceService;
class FileService;

using DevicePtr = std::shared_ptr<Device>;
using FilePtr = std::shared_ptr<File>;
using TrackPtr = std::shared_ptr<Track>;

using DeviceList = std::vector<DevicePtr>;
using FileList = std::vector<FilePtr>;
using TrackList = std::vector<TrackPtr>;

/** @class AudioSession
 *  @brief An audio session that manages devices, files, and tracks. It is the entry point for interacting with the audio engine.
 */
class AudioSession
{
public:
  AudioSession();
  ~AudioSession();

  // Devices
  DeviceList get_audio_devices() const;
  DeviceList get_midi_devices() const;

  DevicePtr get_audio_device(unsigned int device_id) const;
  DevicePtr get_midi_device(unsigned int device_id) const;

  DevicePtr get_default_audio_input_device() const;
  DevicePtr get_default_audio_output_device() const;

  // Files
  FileList get_audio_files(const std::filesystem::path& directory) const;
  FileList get_midi_files(const std::filesystem::path& directory) const;

  FilePtr get_audio_file(const std::filesystem::path& file_path) const;
  FilePtr get_midi_file(const std::filesystem::path& file_path) const;

  // Tracks
  TrackList get_tracks() const;
  TrackPtr add_track() const;

  // Control
  bool play();
  bool record();
  bool stop();

private:
  std::unique_ptr<DeviceService> p_device_service;
  std::unique_ptr<FileService> p_file_service;
  std::unique_ptr<TrackService> p_track_service;
};

} // namespace miniaudioengine

#endif // __AUDIO_SESSION_H__