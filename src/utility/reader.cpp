#include "reader.hpp"
#include <istream>
#include <stdexcept>
#include <unordered_set>

#include "utility/graph.hpp"

namespace MDST {

Reader::Reader(std::istream& in)
    : in_(&in) {}

Graph Reader::Read() {
  if (in_ == nullptr) {
    throw std::runtime_error("no stream to read from");
  }

  std::istream& in = *in_;

  uint64_t vertices, edges;
  in >> vertices >> edges;
  std::unordered_set<Vertex> vertex_data;
  for (uint64_t i = 0; i < vertices; ++i) {
    Vertex v;
    in >> v;

    auto [_, success] = vertex_data.emplace(v);
    if (!success) {
      throw std::runtime_error("vertex is duplicated");
    }
  }

  std::vector<MDST::Edge> edge_data(edges);

  for (uint64_t i = 0; i < edges; ++i) {
    in >> edge_data[i].first >> edge_data[i].second;
    if (!vertex_data.contains(edge_data[i].first) || !vertex_data.contains(edge_data[i].second)) {
      throw std::runtime_error("edge links to non-existent vertex");
    }
  }

  Graph result;
  for (Vertex v : vertex_data) {
    result.AddVertex(v);
  }
  for (Edge e : edge_data) {
    result.AddEdge(e);
  }
  return std::move(result);
}

}  // namespace MDST