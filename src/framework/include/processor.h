#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <string>
#include <memory>

namespace miniaudioengine::framework
{

namespace adapters
{
using AudioStreamStatus = unsigned int;
}

/** @class IProcessor
 *  @brief This is an abstract interface designated the derived object is an audio or MIDI I/O interface.
 */
class IProcessor
{
public:
  IProcessor() = default;
  virtual ~IProcessor() = default;

  virtual void audio_input_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                                   double stream_time, adapters::AudioStreamStatus status, void *user_data) noexcept
  {
    (void)output_buffer;
    (void)input_buffer;
    (void)n_frames;
    (void)stream_time;
    (void)status;
    (void)user_data;
  }

  virtual void audio_output_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                                    double stream_time, adapters::AudioStreamStatus status, void *user_data) noexcept
  {
    (void)output_buffer;
    (void)input_buffer;
    (void)n_frames;
    (void)stream_time;
    (void)status;
    (void)user_data;
  }

  virtual void midi_input_callback(double deltatime, std::vector<unsigned char> *message, void *user_data)
  {
    (void)deltatime;
    (void)user_data;
    (void)message;
  }

  virtual void midi_output_callback(double deltatime, std::vector<unsigned char> *message, void *user_data)
  {
    (void)deltatime;
    (void)user_data;
    (void)message;
  }

  virtual std::string to_string() const = 0;
};

using IProcessorPtr = std::shared_ptr<IProcessor>;

} // namespace miniaudioengine::framework

#endif // __PROCESSOR_H__