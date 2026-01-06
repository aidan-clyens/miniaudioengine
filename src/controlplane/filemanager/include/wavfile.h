#ifndef __WAV_FILE_H__
#define __WAV_FILE_H__

#include <filesystem>
#include <memory>
#include <string>
#include <sndfile.h>
#include <vector>

#include "filemanager.h"

namespace MinimalAudioEngine::Control
{

/** @class AudioFile
 *  @brief Class for handling audio file operations.
 */
class WavFile : public File
{
friend class FileManager;

public:
  virtual ~WavFile() = default;

  unsigned int get_total_frames() const
  {
    return (unsigned int)m_sfinfo.frames;
  }

  unsigned int get_bits_per_sample() const
  {
    int format = m_sfinfo.format & SF_FORMAT_SUBMASK;
    switch (format)
    {
      case SF_FORMAT_PCM_16:
        return 16;
      case SF_FORMAT_PCM_24:
        return 24;
      case SF_FORMAT_PCM_32:
        return 32;
      case SF_FORMAT_FLOAT:
        return 32;
      case SF_FORMAT_DOUBLE:
        return 64;
      default:
        return 0; // Unknown or unsupported format
    }
  }

  unsigned int get_sample_rate() const
  {
    return (unsigned int)m_sfinfo.samplerate;
  }

  unsigned int get_channels() const
  {
    return (unsigned int)m_sfinfo.channels;
  }

  double get_duration_seconds() const
  {
    return static_cast<double>(get_total_frames()) / static_cast<double>(get_sample_rate());
  }

  unsigned int get_format() const
  {
    return (unsigned int)m_sfinfo.format;
  }

  std::string get_format_string() const
  {
    switch (m_sfinfo.format & SF_FORMAT_TYPEMASK)
    {
      case SF_FORMAT_WAV:
        return "WAV";
      case SF_FORMAT_AIFF:
        return "AIFF";
      case SF_FORMAT_FLAC:
        return "FLAC";
      default:
        return "Unknown";
    }
  }

  sf_count_t read_frames(std::vector<float>& buffer, sf_count_t frames_to_read);

  void seek(sf_count_t frame_offset)
  {
    sf_seek(m_sndfile.get(), frame_offset, SEEK_SET);
  }

  std::string to_string() const override
  {
    return "WavFile(Path=" + m_filepath.string() +
           ", TotalFrames=" + std::to_string(get_total_frames()) +
           ", DurationSeconds=" + std::to_string(get_duration_seconds()) +
           ", Format=" + get_format_string() +
           ", SampleRate=" + std::to_string(get_sample_rate()) +
           ", BitsPerSample=" + std::to_string(get_bits_per_sample()) +
           ", Channels=" + std::to_string(get_channels()) + ")";
  }

private:
  WavFile(const std::filesystem::path &path);

  SF_INFO m_sfinfo;
  std::shared_ptr<SNDFILE> m_sndfile;
};

}  // namespace MinimalAudioEngine::Control

#endif  // __WAV_FILE_H__