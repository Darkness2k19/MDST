#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace MDST {

using Vertex = uint64_t;
using Edge = std::pair<Vertex, Vertex>;

class Graph {
private:
  struct EdgeHasher {
    uint64_t operator()(const Edge&) const;
  };

public:
  using EdgeContainer = std::unordered_set<Edge, EdgeHasher>;

public:
  Graph() = default;
  Graph(std::vector<std::vector<Vertex>>);
  Graph(std::vector<Edge>);

  Graph(const Graph&) = default;
  Graph(Graph&&) = default;

  Graph& operator=(const Graph&) = default;
  Graph& operator=(Graph&&) = default;

  bool HasEdge(Edge) const;
  bool AddEdge(Edge);
  bool RemoveEdge(Edge);

  bool AddVertex(Vertex);
  bool RemoveVertex(Vertex);

  bool IsLinked() const;

  uint64_t GetDegree(Vertex) const;
  Vertex GetVertexOfMaximumDegree() const;

  const std::unordered_set<Vertex>& GetVertices() const;
  const EdgeContainer& GetEdges() const;
  const std::unordered_set<Vertex>& GetAdjacentVertices(Vertex) const;

private:
  std::unordered_map<Vertex, std::unordered_set<Vertex>> data_;
  std::unordered_set<Vertex> vertices_;
  EdgeContainer edges_;
};

};  // namespace MDST