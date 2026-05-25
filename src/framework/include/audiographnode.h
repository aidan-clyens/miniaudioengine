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

  /** @brief Get parent node (nullptr if root)
   *  @return Shared pointer to parent node.
   */
  IAudioGraphNodePtr get_parent() const { return p_parent; }
  
  /** @brief Get child nodes.
   *  @return Vector of shared pointers to child nodes.
   */
  std::vector<IAudioGraphNodePtr> get_children() const { return m_children; }

  /** @brief Add a child node.
   *  @param child Shared pointer to the node to add.
   */
  void add_child(IAudioGraphNodePtr child)
  {
    if (child)
    {
      child->p_parent = shared_from_this();
      m_children.push_back(child);
    }
  }

  /** @brief Remove a child node.
   *  @param child Shared pointer to the child node to remove.
   */
  void remove_child(IAudioGraphNodePtr child)
  {
    if (child)
    {
      child->p_parent.reset();
      m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
    }
  }

  virtual std::string to_string() const = 0;

protected:

  void children_to_string(std::string &str) const
  {
    size_t count = 0;
    str += "[";
    for (const framework::IAudioGraphNodePtr child : get_children())
    {
      if (count++)
      {
        str += ", ";
      }

      str += child->to_string();
    }
    str += "]";
  }

private:
  IAudioGraphNodePtr p_parent;
  std::vector<IAudioGraphNodePtr> m_children;
};

} // namespace miniaudioengine::framework

#endif // __AUDIO_GRAPH_NODE_H__