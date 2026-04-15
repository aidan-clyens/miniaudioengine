#include "sampleplayer.h"

#include <string>

using namespace miniaudioengine::audio;

void SamplePlayer::process_audio(float *output_buffer, unsigned int channels, unsigned int n_frames, double stream_time) noexcept
{
  (void)output_buffer; // Unused for now, but can be used to write audio data to hardware output
  (void)channels; // Unused for now, but can be used to handle different channel configurations in the future
  (void)n_frames; // Unused for now, but can be used to handle different buffer sizes in the future
  (void)stream_time; // Unused for now, but can be used to implement time-based processing in the future
}

void SamplePlayer::reset()
{

}

std::string SamplePlayer::to_string() const
{
  return "SamplePlayer";
}
