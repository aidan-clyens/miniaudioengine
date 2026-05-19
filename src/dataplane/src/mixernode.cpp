#include "mixernode.h"

using namespace miniaudioengine::dataplane;

std::string MixerNode::to_string() const
{
  std::string str = "MixerNode(";
  children_to_string(str);
  str += ")";
  return str;
}
