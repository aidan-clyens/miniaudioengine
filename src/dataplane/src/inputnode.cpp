#include "inputnode.h"

using namespace miniaudioengine::dataplane;

std::string InputNode::to_string() const
{
  std::string str = "InputNode(";
  children_to_string(str);
  str += ")";
  return str;
}
