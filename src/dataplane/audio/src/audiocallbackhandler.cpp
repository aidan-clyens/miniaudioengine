#include "audiocallbackhandler.h"

#include "trackaudiodataplane.h"
#include "logger.h"

using namespace MinimalAudioEngine;

int AudioCallbackHandler::audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                          double stream_time, RtAudioStreamStatus status, void *user_data) noexcept
{
  AudioCallbackContext* context = static_cast<AudioCallbackContext*>(user_data);
  if (!context)
  {
    LOG_ERROR("AudioCallbackHandler: No valid AudioCallbackContext provided.");
    return 1;
  }

  for (const auto& track_data : context->active_tracks)
  {
    track_data->process_audio(output_buffer, input_buffer, n_frames, stream_time, status);
  }
  return 0;
}