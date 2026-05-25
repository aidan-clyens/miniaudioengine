#ifndef __OUTPUT_NODE_H__
#define __OUTPUT_NODE_H__

#include "audiographnode.h"
#include "io.h"

namespace miniaudioengine::dataplane
{

class OutputNode : public framework::IAudioGraphNode
{
public:
  OutputNode(framework::IInputOutputPtr io) : p_io(io) {}
  ~OutputNode() = default;

  framework::IInputOutputPtr get_io() const { return p_io; }

  std::string to_string() const override;

private:
  framework::IInputOutputPtr p_io;
};

using OutputNodePtr = std::shared_ptr<OutputNode>;

} // namespace miniaudioengine::dataplane

#endif // __OUTPUT_NODE_H__