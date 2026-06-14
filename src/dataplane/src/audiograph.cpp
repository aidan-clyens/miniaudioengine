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

OutputNodePtr AudioGraph::add_output_node(IInputOutputPtr output, IAudioGraphNodePtr parent)
{
  auto node = std::make_shared<OutputNode>(output);
  return std::dynamic_pointer_cast<OutputNode>(add_node(node, parent));
}

ProcessorNodePtr AudioGraph::add_processor_node(IAudioGraphNodePtr parent)
{
  auto node = std::make_shared<ProcessorNode>();
  return std::dynamic_pointer_cast<ProcessorNode>(add_node(node, parent));
}

IAudioGraphNodePtr AudioGraph::add_node(IAudioGraphNodePtr node, IAudioGraphNodePtr parent)
{
  if (!node)
  {
    LOG_ERROR("AudioGraph: Attempting to add null IAudioGraphNodePtr node!");
    return nullptr;
  }

  size_t node_index = IGraph::add_node(node);
  node->set_index(node_index);

  if (parent)
  {
    IGraph::add_edge(parent->get_index(), node->get_index());
  }

  LOG_INFO("AudioGraph: Added ", node->to_string());
  return node;
}

std::string AudioGraph::to_string() const
{
  std::string str = "AudioGraph(";
  str += ")";
  return str;
}

} // namespace miniaudioengine::dataplane
