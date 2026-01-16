#ifndef __AUDIO_CALLBACK_HANDLER_H__
#define __AUDIO_CALLBACK_HANDLER_H__

#include <rtaudio/RtAudio.h>

namespace miniaudioengine::data
{

typedef std::shared_ptr<class AudioDataPlane> TrackAudioDataplanePtr;

struct AudioCallbackContext
{
  std::vector<TrackAudioDataplanePtr> active_tracks;
};

class AudioCallbackHandler
{
public:

  static int audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                    double stream_time, RtAudioStreamStatus status, void *user_data) noexcept;
};

} // namespace miniaudioengine::data

#endif // __AUDIO_CALLBACK_HANDLER_H__