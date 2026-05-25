#include "audiograph.h"
#include "mixernode.h"
#include "outputnode.h"
#include "processornode.h"
#include "inputnode.h"
#include "logger.h"

namespace miniaudioengine::dataplane
{

MixerNodePtr AudioGraph::add_mixer_node(IAudioGraphNodePtr parent)
{
  auto node = std::make_shared<MixerNode>();
  return std::dynamic_pointer_cast<MixerNode>(add_node(node, parent));
}

InputNodePtr AudioGraph::add_input_node(IInputOutputPtr input, IAudioGraphNodePtr parent)
{
  auto node = std::make_shared<InputNode>(input);
  return std::dynamic_pointer_cast<InputNode>(add_node(node, parent));
}

OutputNodePtr AudioGraph::add_output_node(IAudioGraphNodePtr parent)
{
  auto node = std::make_shared<OutputNode>();
  return std::dynamic_pointer_cast<OutputNode>(add_node(node, parent));
}

ProcessorNodePtr AudioGraph::add_processor_node(IAudioGraphNodePtr parent)
{
  auto node = std::make_shared<ProcessorNode>();
  return std::dynamic_pointer_cast<ProcessorNode>(add_node(node, parent));
}

IAudioGraphNodePtr AudioGraph::get_root_node() const
{
  return p_root_node;
}

std::vector<IAudioGraphNodePtr> AudioGraph::get_leaf_nodes() const
{
  if (p_root_node == nullptr)
  {
    LOG_WARNING("AudioGraph: No root node");
    return std::vector<IAudioGraphNodePtr>();
  }

  std::vector<IAudioGraphNodePtr> leaf_nodes;    
  get_leaf_nodes_impl(p_root_node, leaf_nodes);
  return leaf_nodes;
}

IAudioGraphNodePtr AudioGraph::add_node(IAudioGraphNodePtr node, IAudioGraphNodePtr parent)
{
  if (!node)
  {
    LOG_ERROR("AudioGraph: Attempting to add null IAudioGraphNodePtr node!");
    return nullptr;
  }

  if (parent)
  {
    parent->add_child(node);
  }
  else
  {
    // If a parent node is not passed, this node must be the root
    p_root_node = node;
  }
  LOG_INFO("AudioGraph: Added ", node->to_string());
  return node;
}

std::string AudioGraph::to_string() const
{
  std::string str = "AudioGraph(";

  if (p_root_node)
  {
    str += p_root_node->to_string();
  }

  str += ")";
  return str;
}

void AudioGraph::get_leaf_nodes_impl(IAudioGraphNodePtr parent, std::vector<IAudioGraphNodePtr> &leaf_nodes) const
{
  if (parent == nullptr)
  {
    LOG_WARNING("AudioGraph: get_leaf_nodes_impl - Parent node is null");
    return;
  }

  // If parent has no children, add to leaf nodes list and return
  if (parent->get_children().size() == 0)
  {
    LOG_DEBUG("AudioGraph: get_leaf_nodes_impl - Found leaf node ", parent->to_string());
    leaf_nodes.push_back(parent);
    return;
  }

  // Else, iterate through children
  for (auto child : parent->get_children())
  {
    get_leaf_nodes_impl(child, leaf_nodes);
  }
}

} // namespace miniaudioengine::dataplane
