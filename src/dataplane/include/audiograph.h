#ifndef __AUDIO_GRAPH_H__
#define __AUDIO_GRAPH_H__

#include "audiographnode.h"

#include <memory>
#include <string>

namespace miniaudioengine::dataplane
{

// Forward declarations
class MixerNode;
class InputNode;
class OutputNode;
class ProcessorNode;

using IAudioGraphNodePtr = framework::IAudioGraphNodePtr;
using MixerNodePtr = std::shared_ptr<MixerNode>;
using InputNodePtr = std::shared_ptr<InputNode>;
using OutputNodePtr = std::shared_ptr<OutputNode>;
using ProcessorNodePtr = std::shared_ptr<ProcessorNode>;

/** @class AudioGraph
 *  @brief Represents the audio processing graph.
 */
class AudioGraph
{
public:
  AudioGraph() = default;
  ~AudioGraph() = default;

  MixerNodePtr add_mixer_node(IAudioGraphNodePtr parent = nullptr);
  InputNodePtr add_input_node(IAudioGraphNodePtr parent = nullptr);
  OutputNodePtr add_output_node(IAudioGraphNodePtr parent = nullptr);
  ProcessorNodePtr add_processor_node(IAudioGraphNodePtr parent = nullptr);

  std::string to_string() const;

private:
  IAudioGraphNodePtr add_node(IAudioGraphNodePtr node, IAudioGraphNodePtr parent = nullptr);

private:
  IAudioGraphNodePtr p_parent_node;
};

} // namespace miniaudioengine::dataplane

#endif // __AUDIO_GRAPH_H__