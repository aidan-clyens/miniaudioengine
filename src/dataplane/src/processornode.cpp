#include "processornode.h"

using namespace miniaudioengine::dataplane;

std::string ProcessorNode::to_string() const
{
  std::string str = "ProcessorNode(";
  children_to_string(str);
  str += ")";
  return str;
}
