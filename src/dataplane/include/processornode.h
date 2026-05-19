#ifndef __PROCESSOR_NODE_H__
#define __PROCESSOR_NODE_H__

#include "audiographnode.h"

#include <memory>

namespace miniaudioengine::dataplane
{

class ProcessorNode : public framework::IAudioGraphNode
{
public:
  ProcessorNode() = default;
  ~ProcessorNode() = default;

  std::string to_string() const override;
};

using ProcessorNodePtr = std::shared_ptr<ProcessorNode>;

} // namespace miniaudioengine::dataplane

#endif // __PROCESSOR_NODE_H__