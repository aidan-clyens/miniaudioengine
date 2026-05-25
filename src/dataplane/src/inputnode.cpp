#include "inputnode.h"

using namespace miniaudioengine::dataplane;

std::string InputNode::to_string() const
{
  std::string str = "InputNode(";
  if (p_io) {
    str += "io=" + p_io->to_string();
    str += ", ";
  }
  children_to_string(str);
  str += ")";
  return str;
}
