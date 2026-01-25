#ifndef __SAMPLER_PLAYER_H__
#define __SAMPLER_PLAYER_H__

#include <unordered_map>
#include <memory>

#include "audioprocessor.h"

// Forward declaration
namespace miniaudioengine::data
{
  enum class eMidiNoteValues : int;
} // namespace miniaudioengine::data

namespace miniaudioengine::processing
{

// Forward declaration
class Sample;
using SamplePtr = std::shared_ptr<Sample>;

class SamplePlayer : public IAudioProcessor
{
public:
  SamplePlayer() = default;

  void process_audio(float *output_buffer, unsigned int channels, unsigned int n_frames, double stream_time) noexcept override;

  void reset() override;

  /** @brief Adds a sample associated with a specific MIDI note value
   *  @param sample Shared pointer to the Sample object
   *  @param note MIDI note value associated with the sample
   */
  void add_sample(const SamplePtr &sample, const data::eMidiNoteValues note)
  {
    m_samples[note] = sample;
  }

  std::string to_string() const override;

private:
  std::unordered_map<data::eMidiNoteValues, SamplePtr> m_samples;

private:
  void process() override
  {
    // Implementation of the processing loop goes here
  }
};

using SamplePlayerPtr = std::shared_ptr<SamplePlayer>;

} // namespace miniaudioengine::processing

#endif // __SAMPLER_PLAYER_H__