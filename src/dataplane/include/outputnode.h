#ifndef __OUTPUT_NODE_H__
#define __OUTPUT_NODE_H__

#include "audiographnode.h"

namespace miniaudioengine::dataplane
{

class OutputNode : public framework::IAudioGraphNode
{
public:
  OutputNode() = default;
  ~OutputNode() = default;

  std::string to_string() const override;
};

using OutputNodePtr = std::shared_ptr<OutputNode>;

} // namespace miniaudioengine::dataplane

#endif // __OUTPUT_NODE_H__