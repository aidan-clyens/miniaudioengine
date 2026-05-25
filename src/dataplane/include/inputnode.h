#ifndef __INPUT_NODE_H__
#define __INPUT_NODE_H__

#include "audiographnode.h"
#include "io.h"

#include <memory>

namespace miniaudioengine::dataplane
{

/** @class InputNode
 *  @brief Audio/MIDI data input for the AudioGraph dataplane 
 */
class InputNode : public framework::IAudioGraphNode
{
public:
  InputNode(framework::IInputOutputPtr io): p_io(io) {}
  ~InputNode() = default;

  framework::IInputOutputPtr get_io() const { return p_io; }

  std::string to_string() const override;

private:
  framework::IInputOutputPtr p_io;
};

using InputNodePtr = std::shared_ptr<InputNode>;

} // namespace miniaudioengine::dataplane

#endif // __INPUT_NODE_H__