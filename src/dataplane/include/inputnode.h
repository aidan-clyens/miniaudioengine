#ifndef __INPUT_NODE_H__
#define __INPUT_NODE_H__

#include "audiographnode.h"

#include <memory>

namespace miniaudioengine::dataplane
{

class InputNode : public framework::IAudioGraphNode
{
public:
  InputNode() = default;
  ~InputNode() = default;

  std::string to_string() const override;
};

using InputNodePtr = std::shared_ptr<InputNode>;

} // namespace miniaudioengine::dataplane

#endif // __INPUT_NODE_H__