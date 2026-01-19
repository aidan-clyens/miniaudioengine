#ifndef __AUDIO_PROCESSOR_H__
#define __AUDIO_PROCESSOR_H__

#include "processor.h"

namespace miniaudioengine::processing
{

class IAudioProcessor : public core::IProcessor
{
public:
  virtual ~IAudioProcessor() = default;

  // Add audio-specific processing functionality here
};

};

#endif // __AUDIO_PROCESSOR_H__