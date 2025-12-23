#ifndef __TRACK_AUDIO_DATA_PLANE_H__
#define __TRACK_AUDIO_DATA_PLANE_H__

#include "lockfree_ringbuffer.h"

#include <string>
#include <memory>

#include <rtaudio/RtAudio.h>

#define BUFFER_FRAMES 512

namespace MinimalAudioEngine
{

class TrackAudioDataPlane
{
public:
  TrackAudioDataPlane() = default;
  virtual ~TrackAudioDataPlane() = default;

  void process_audio(void *output_buffer, void *input_buffer, unsigned int n_frames,
                     double stream_time, RtAudioStreamStatus status) noexcept;

  std::string to_string() const
  {
    return "TrackAudioDataPlane";
  }

private:
  LockfreeRingBuffer<float, BUFFER_FRAMES> m_input_buffer;
};

typedef std::shared_ptr<TrackAudioDataPlane> TrackAudioDataPlanePtr;

} // namespace MinimalAudioEngine

#endif // __TRACK_AUDIO_DATA_PLANE_H__