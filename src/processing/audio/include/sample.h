#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include <vector>
#include <memory>
#include <filesystem>

#include "wavfile.h"

namespace miniaudioengine::processing
{

/** @struct Sample 
 *  @brief Represents an audio sample in the processing module.
 */
struct Sample
{
  std::string name;
  std::filesystem::path filepath;
  std::vector<float> data;
  unsigned int sample_rate;
  unsigned int channels;
  unsigned int total_frames;

  Sample(const file::WavFilePtr &wav_file):
    name(wav_file->get_filename()),
    filepath(wav_file->get_filepath()),
    sample_rate(wav_file->get_sample_rate()),
    channels(wav_file->get_channels()),
    total_frames(wav_file->get_total_frames())
  {
    // Read all frames from the WAV file
    data.resize(total_frames * channels);
    wav_file->read_frames(data, total_frames);
  }

  std::string to_string() const
  {
    return "Sample(Name=" + name +
           ", Filepath=" + filepath.string() +
           ", SampleRate=" + std::to_string(sample_rate) +
           ", Channels=" + std::to_string(channels) +
           ", TotalFrames=" + std::to_string(total_frames) + ")";
  }
};

std::ostream& operator<<(std::ostream& os, const Sample& sample)
{
  os << sample.to_string();
  return os;
}

using SamplePtr = std::shared_ptr<Sample>;

} // namespace miniaudioengine::processing

#endif // __SAMPLE_H__