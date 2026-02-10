#include "audiocallbackhandler.h"

#include "audiodataplane.h"
#include "logger.h"

#include <algorithm>

using namespace miniaudioengine::data;
using namespace miniaudioengine::core;

int AudioCallbackHandler::audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                          double stream_time, RtAudioStreamStatus status, void *user_data) noexcept
{
  set_thread_name("RtAudioCallback");

  AudioCallbackContext* context = static_cast<AudioCallbackContext*>(user_data);
  if (!context)
  {
    LOG_ERROR("AudioCallbackHandler: No valid AudioCallbackContext provided.");
    return 1;
  }

  float *out_buffer = static_cast<float *>(output_buffer);
  if (out_buffer == nullptr)
  {
    return 0;
  }

  if (context->active_tracks.empty())
  {
    return 0;
  }

  const unsigned int output_channels = context->active_tracks.front()->get_output_channels();
  const size_t total_samples = static_cast<size_t>(n_frames) * output_channels;

  if (total_samples == 0)
  {
    return 0;
  }

  std::fill(out_buffer, out_buffer + total_samples, 0.0f);

  for (const auto& track_data : context->active_tracks)
  {
    track_data->process_audio(nullptr, input_buffer, n_frames, stream_time, status);

    const auto& track_buffer = track_data->get_output_buffer();
    const size_t mix_samples = std::min(track_buffer.size(), total_samples);
    for (size_t i = 0; i < mix_samples; ++i)
    {
      out_buffer[i] += track_buffer[i];
    }
  }
  return 0;
}