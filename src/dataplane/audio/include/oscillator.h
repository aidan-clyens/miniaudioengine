#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include <string>
#include <cmath>

#include "audioprocessor.h"
#include "logger.h"

namespace MinimalAudioEngine
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/** @class OscillatorAudioProcessor 
 *  @brief Simple audio processor that generates a sine wave oscillator.
 */
class OscillatorAudioProcessor : public MinimalAudioEngine::IAudioProcessor
{
public:
  OscillatorAudioProcessor() = default;
  ~OscillatorAudioProcessor() override = default;

  void set_frequency(const float frequency)
  {
    m_frequency = frequency;
  }

  void get_next_audio_frame(float *output_buffer, unsigned int frames, unsigned int channels, unsigned int sample_rate) override
  {
    LOG_INFO("Generating oscillator audio frame: " +
             std::to_string(frames) + " frames, " +
             std::to_string(channels) + " channels, " +
             std::to_string(sample_rate) + " Hz");

    for (unsigned int i = 0; i < frames; ++i)
    {
      float sample = static_cast<float>(std::sin(m_phase));
      m_phase += (2.0 * M_PI * m_frequency) / static_cast<float>(sample_rate);
      if (m_phase >= (2.0 * M_PI))
      {
        m_phase -= (2.0 * M_PI);
      }

      for (unsigned int ch = 0; ch < channels; ++ch)
      {
        output_buffer[i * channels + ch] = sample;
      }
    }
  }

  std::string to_string() const override
  {
    return "OscillatorAudioProcessor(Frequency=" + std::to_string(m_frequency) + " Hz)";
  }

private:
  float m_frequency = 440.0f; // A4
  float m_phase = 0.0f;
};

} // namespace MinimalAudioEngine

#endif // __OSCILLATOR_H__