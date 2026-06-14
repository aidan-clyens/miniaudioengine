#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <vector>
#include <string>

namespace miniaudioengine::framework
{

template <typename T>
class IGraph
{
public:
  size_t add_node(T value)
  {
    m_nodes.push_back(value);
    m_adjacency.emplace_back();
    return m_nodes.size() - 1;
  }

  void add_edge(size_t parent, size_t child)
  {
    m_adjacency[parent].push_back(child);
  }

  T get_root_node() { return m_nodes[0]; }

  std::vector<T> get_leaf_nodes()
  {
    std::vector<T> leaf_nodes;
    // Leaf nodes have no outgoing edges
    for (size_t i = 0; i < m_adjacency.size(); i++)
    {
      if (m_adjacency[i].size() == 0)
      {
        leaf_nodes.push_back(m_nodes[i]);
      }
    }

    return leaf_nodes;
  }

private:
  std::vector<T> m_nodes;
  std::vector<std::vector<size_t>> m_adjacency;
};

} // namespace miniaudioengine::framework

#endif // __GRAPH_H__