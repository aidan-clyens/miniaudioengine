#include "outputnode.h"

using namespace miniaudioengine::dataplane;

std::string OutputNode::to_string() const
{
  std::string str = "OutputNode(";
  children_to_string(str);
  str += ")";
  return str;
}
