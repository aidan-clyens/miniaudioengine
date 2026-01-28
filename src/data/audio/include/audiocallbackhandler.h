#ifndef __AUDIO_CALLBACK_HANDLER_H__
#define __AUDIO_CALLBACK_HANDLER_H__

#include <rtaudio/RtAudio.h>

namespace miniaudioengine::data
{
// Forward declaration
typedef std::shared_ptr<class AudioDataPlane> AudioDataPlanePtr;

struct AudioCallbackContext
{
  std::vector<AudioDataPlanePtr> active_tracks;
};

class AudioCallbackHandler
{
public:

  static int audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                    double stream_time, RtAudioStreamStatus status, void *user_data) noexcept;
};

} // namespace miniaudioengine::data

#endif // __AUDIO_CALLBACK_HANDLER_H__