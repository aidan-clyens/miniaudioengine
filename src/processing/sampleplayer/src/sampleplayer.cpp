#include "sampleplayer.h"

#include <string>

using namespace miniaudioengine::audio;

void SamplePlayer::process_audio(float *output_buffer, unsigned int channels, unsigned int n_frames, double stream_time) noexcept
{

}

void SamplePlayer::reset()
{

}

std::string SamplePlayer::to_string() const
{
  return "SamplePlayer";
}
