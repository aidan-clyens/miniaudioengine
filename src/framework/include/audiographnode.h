#ifndef __AUDIO_GRAPH_NODE_H__
#define __AUDIO_GRAPH_NODE_H__

#include "logger.h"

#include <vector>
#include <memory>
#include <string>

namespace miniaudioengine::framework
{

using IAudioGraphNodePtr = std::shared_ptr<class IAudioGraphNode>;

/** @class IAudioGraphNode
 *  @brief Interface for Audio Graph nodes. Allows one parent and multiple children, forming a tree structure.
 */
class IAudioGraphNode : public std::enable_shared_from_this<IAudioGraphNode>
{
public:
  virtual ~IAudioGraphNode() = default;

  void set_index(const size_t index) { m_index = index; }
  size_t get_index() const { return m_index; }

  virtual std::string to_string() const = 0;

private:
  size_t m_index;
};

} // namespace miniaudioengine::framework

#endif // __AUDIO_GRAPH_NODE_H__