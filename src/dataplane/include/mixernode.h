#ifndef __MIXER_NODE_H__
#define __MIXER_NODE_H__

#include "audiographnode.h"

#include <memory>

namespace miniaudioengine::dataplane
{

class MixerNode : public framework::IAudioGraphNode
{
public:
  MixerNode() = default;
  ~MixerNode() = default;

  std::string to_string() const override;
};

using MixerNodePtr = std::shared_ptr<MixerNode>;

} // namespace miniaudioengine::dataplane

#endif // __MIXER_NODE_H__