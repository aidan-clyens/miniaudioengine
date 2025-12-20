#ifndef __AUDIO_PROCESSOR_H__
#define __AUDIO_PROCESSOR_H__

#include <string>
#include <ostream>

namespace MinimalAudioEngine
{

/** @class IAudioProcessor
 *  @brief Interface for audio processing nodes.
 */
class IAudioProcessor
{
public:
  virtual ~IAudioProcessor() = default;

  /** @brief Process the next audio frame.
   *  @param output_buffer The output buffer to fill with audio data.
   *  @param frames The number of frames to process.
   *  @param channels The number of channels.
   *  @param sample_rate The sample rate.
   */
  virtual void get_next_audio_frame(float *output_buffer, unsigned int frames, unsigned int channels, unsigned int sample_rate) = 0;

  virtual std::string to_string() const
  {
    return "IAudioProcessor";
  }
};

} // namespace MinimalAudioEngine

#endif // __AUDIO_PROCESSOR_H__