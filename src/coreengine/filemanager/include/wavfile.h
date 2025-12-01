#ifndef __WAV_FILE_H__
#define __WAV_FILE_H__

#include <filesystem>
#include <memory>
#include <string>
#include <sndfile.h>

#include "filemanager.h"

namespace Files
{

/** @class AudioFile
 *  @brief Class for handling audio file operations.
 */
class WavFile : public File
{
friend class FileManager;

public:
  virtual ~WavFile() = default;

  unsigned int get_sample_rate() const
  {
    return (unsigned int)m_sfinfo.samplerate;
  }

  unsigned int get_channels() const
  {
    return (unsigned int)m_sfinfo.channels;
  }

  unsigned int get_format() const
  {
    return (unsigned int)m_sfinfo.format;
  }

private:
  WavFile(const std::filesystem::path &path);

  SF_INFO m_sfinfo;
  std::shared_ptr<SNDFILE> m_sndfile;
};

}  // namespace Files

#endif  // __WAV_FILE_H__