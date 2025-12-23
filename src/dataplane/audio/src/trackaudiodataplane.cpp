#include "trackaudiodataplane.h"

using namespace MinimalAudioEngine;

void TrackAudioDataPlane::process_audio(void *output_buffer, void *input_buffer, unsigned int n_frames,
                                        double stream_time, RtAudioStreamStatus status) noexcept
{
  if (input_buffer != nullptr)
  {
    float *in_buffer = static_cast<float *>(input_buffer);
    m_input_buffer.try_push(in_buffer, static_cast<size_t>(n_frames));
  }

  // TODO - Handler processing

  // TODO - Copy to output buffer
  if (output_buffer != nullptr)
  {
    float *out_buffer = static_cast<float *>(output_buffer);
    size_t frames_copied = m_input_buffer.try_pop(out_buffer, static_cast<size_t>(n_frames));

    // Zero-fill remaining frames if buffer didn't have enough data
    if (frames_copied < n_frames)
    {
      std::memset(out_buffer + frames_copied, 0, (n_frames - frames_copied) * sizeof(float));
    }
  }
}