#include "graph.hpp"

#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

namespace MDST {

uint64_t Graph::EdgeHasher::operator()(const Edge& e) const {
  return e.first * 998244353 + e.second;
}

Graph::Graph(std::vector<std::vector<Vertex>> data) {
  for (uint64_t v = 0; v < data.size(); ++v) {
    for (Vertex to : data[v]) {
      AddEdge(std::make_pair(v, to));
    }
  }
}

Graph::Graph(std::vector<Edge> edges) {
  for (Edge e : edges) {
    AddEdge(e);
  }
}

bool Graph::HasEdge(Edge e) const {
  return edges_.contains(e) ||
         edges_.contains(std::make_pair(e.second, e.first));
}

bool Graph::AddEdge(Edge e) {
  if (HasEdge(e)) {
    return false;
  }

  AddVertex(e.first);
  AddVertex(e.second);
  data_[e.first].insert(e.second);
  data_[e.second].insert(e.first);
  edges_.insert(e);
  return true;
}

bool Graph::RemoveEdge(Edge e) {
  if (!HasEdge(e)) {
    return false;
  }

  data_[e.first].erase(e.second);
  data_[e.second].erase(e.first);
  edges_.erase(e);
  edges_.erase(std::make_pair(e.second, e.first));
  return true;
}

bool Graph::AddVertex(Vertex v) {
  if (vertices_.contains(v)) {
    return false;
  }

  vertices_.insert(v);
  data_.emplace(v, std::unordered_set<Vertex>{});
  return true;
}

bool Graph::RemoveVertex(Vertex v) {
  if (!vertices_.contains(v)) {
    return false;
  }

  std::vector<Edge> edges;
  for (Vertex to : GetAdjacentVertices(v)) {
    edges.emplace_back(v, to);
  }
  for (Edge e : edges) {
    RemoveEdge(e);
  }
  vertices_.erase(v);
  data_.erase(v);
  return true;
}

bool Graph::IsLinked() const {
  if (vertices_.size() == 0) {
    return true;
  }

  Vertex start = *vertices_.begin();
  std::queue<Vertex> q;
  std::unordered_set<Vertex> visited;
  q.push(start);
  visited.insert(start);

  while (q.size()) {
    Vertex now = q.front();
    q.pop();

    for (Vertex v : GetAdjacentVertices(now)) {
      if (visited.contains(v)) {
        continue;
      }

      visited.insert(v);
      q.push(v);
    }
  }
  return visited.size() == GetVertices().size();
}

uint64_t Graph::GetDegree(Vertex v) const {
  return GetAdjacentVertices(v).size();
}

Vertex Graph::GetVertexOfMaximumDegree() const {
  if (vertices_.size() == 0) {
    throw std::runtime_error("Graph is empty");
  }

  Vertex result = *vertices_.begin();
  uint64_t degree = GetDegree(result);
  for (Vertex v : GetVertices()) {
    uint64_t new_degree = GetDegree(v);
    if (new_degree > degree) {
      degree = new_degree;
      result = v;
    }
  }
  return result;
}

const std::unordered_set<Vertex>& Graph::GetVertices() const {
  return vertices_;
}

const Graph::EdgeContainer& Graph::GetEdges() const { return edges_; }

const std::unordered_set<Vertex>& Graph::GetAdjacentVertices(Vertex v) const {
  return data_.at(v);
}

}  // namespace MDST