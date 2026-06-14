#include "outputnode.h"

using namespace miniaudioengine::dataplane;

std::string OutputNode::to_string() const
{
  std::string str = "OutputNode(";
  if (p_io)
  {
    str += "io=" + p_io->to_string();
  }
  str += ")";
  return str;
}
