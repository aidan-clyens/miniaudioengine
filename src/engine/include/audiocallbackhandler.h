#ifndef __AUDIO_CALLBACK_HANDLER_H__
#define __AUDIO_CALLBACK_HANDLER_H__

#include <memory>
#include <vector>

#include "audiodataplane.h"
#include "audioadapter.h"

namespace miniaudioengine::core
{

struct AudioCallbackContext
{
  std::vector<AudioDataPlanePtr> active_tracks;
};

class AudioCallbackHandler
{
public:

  static int audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                    double stream_time, adapter::AudioStreamStatus status, void *user_data) noexcept;
};

} // namespace miniaudioengine::core

#endif // __AUDIO_CALLBACK_HANDLER_H__
