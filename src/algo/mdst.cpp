#include "mdst.hpp"

#include <cstdlib>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "utility/graph.hpp"

namespace MDST {

Graph GetAnyST(const Graph& from) {
  Graph result;
  Vertex start = *from.GetVertices().begin();
  for (Vertex v : from.GetVertices()) {
    result.AddVertex(v);
  }

  std::unordered_set<Vertex> visited;
  std::queue<Vertex> q;

  visited.insert(start);
  q.push(start);

  while (q.size()) {
    Vertex now = q.front();
    q.pop();

    for (Vertex v : from.GetAdjacentVertices(now)) {
      if (visited.contains(v)) {
        continue;
      }

      result.AddEdge(std::make_pair(now, v));

      visited.insert(v);
      q.push(v);
    }
  }

  return std::move(result);
}

void Coloring(Vertex v, const Graph& g,
              const std::unordered_set<Vertex>& forbidden,
              std::unordered_map<Vertex, uint64_t>& colors,
              uint64_t current_color) {
  colors[v] = current_color;
  for (Vertex to : g.GetAdjacentVertices(v)) {
    if (forbidden.contains(to)) {
      continue;
    }
    if (colors.contains(to)) {
      continue;
    }

    colors[to] = current_color;
    Coloring(to, g, forbidden, colors, current_color);
  }
}

std::unordered_map<Vertex, uint64_t> MakeColoring(
    const Graph& g, const std::unordered_set<Vertex>& forbidden,
    const std::unordered_set<Vertex>& starting_vertices) {
  uint64_t current_color = 0;
  std::unordered_map<Vertex, uint64_t> colors;
  for (Vertex v : starting_vertices) {
    if (colors.contains(v)) {
      continue;
    }

    Coloring(v, g, forbidden, colors, current_color++);
  }
  return std::move(colors);
}

bool TryReplaceVertex(Vertex v, const Graph& g, const Graph& prev_iter, Graph& iter) {
  auto colors = MakeColoring(iter, {v}, iter.GetAdjacentVertices(v));

  uint64_t max_degree = prev_iter.GetDegree(v) + 1;

  for (Edge e : g.GetEdges()) {
    if (!colors.contains(e.first) || !colors.contains(e.second) ||
        colors[e.first] == colors[e.second]) {
      continue;
    }
    if (prev_iter.GetDegree(e.first) == max_degree - 1 ||
        prev_iter.GetDegree(e.second) == max_degree - 1) {
      continue;
    }

    iter.AddEdge(e);

    for (Vertex to : iter.GetAdjacentVertices(v)) {
      if (colors[to] == colors[e.first] || colors[to] == colors[e.second]) {
        iter.RemoveEdge(std::make_pair(v, to));
        return true;
      }
    }

    // bug
    std::abort();
  }

  return false;
}

struct PhaseResult {
  Graph iter;
  bool success;
  bool final;
};

PhaseResult Phase(const Graph& g, Graph prev_iter, uint64_t max_degree) {
  Graph iter = prev_iter;
  std::unordered_set<Vertex> forbidden;
  for (Vertex v : prev_iter.GetVertices()) {
    if (prev_iter.GetDegree(v) == max_degree) {
      iter.RemoveVertex(v);
      forbidden.insert(v);
    }
  }

  if (iter.GetVertices().size() == 0) {
    return {std::move(prev_iter), false, false};
  }

  // For quick partition identifying
  auto colors = MakeColoring(iter, forbidden, iter.GetVertices());

  std::optional<Edge> edge_storage;

  // Search for edge with small degrees
  for (Edge e : g.GetEdges()) {
    if (!colors.contains(e.first) || !colors.contains(e.second) ||
        colors[e.first] == colors[e.second]) {
      continue;
    }

    if (prev_iter.GetDegree(e.first) < max_degree - 1 &&
        prev_iter.GetDegree(e.second) < max_degree - 1) {
      edge_storage.emplace(e);
      break;
    }
  }

  if (!edge_storage) {
    // Search for edge with at least one degree k - 1
    for (Edge e : g.GetEdges()) {
      if (!colors.contains(e.first) || !colors.contains(e.second) ||
          colors[e.first] == colors[e.second]) {
        continue;
      }

      if (prev_iter.GetDegree(e.first) == max_degree - 1 &&
          !TryReplaceVertex(e.first, g, prev_iter, iter)) {
        continue;
      }

      if (prev_iter.GetDegree(e.second) == max_degree - 1 &&
          !TryReplaceVertex(e.second, g, prev_iter, iter)) {
        continue;
      }

      edge_storage.emplace(e);
      break;
    }
  }

  if (!edge_storage) {
    return {std::move(prev_iter), false};
  }

  Edge edge = edge_storage.value();
  iter.AddEdge(edge);

  for (Vertex v : forbidden) {
    iter.AddVertex(v);
  }

  // Get all vertices that can be accessed from first vertex of the edge
  // excluding to component of second vertex

  std::unordered_set<Vertex> first_accessed;
  std::queue<Vertex> q;

  first_accessed.insert(edge.first);
  q.push(edge.first);

  while (q.size()) {
    Vertex now = q.front();
    q.pop();

    for (Vertex to : prev_iter.GetAdjacentVertices(now)) {
      if (first_accessed.contains(to)) {
        continue;
      }
      if (colors.contains(to) && colors[to] == colors[edge.second]) {
        continue;
      }

      first_accessed.insert(to);
      q.push(to);
    }
  }

  // Find any vertex from listed above that is accessible from the second vertex

  std::unordered_set<Vertex> visited;

  visited.insert(edge.second);
  q.push(edge.second);

  std::optional<Edge> removed_edge;

  while (q.size()) {
    Vertex now = q.front();
    q.pop();

    for (Vertex to : prev_iter.GetAdjacentVertices(now)) {
      if (visited.contains(to)) {
        continue;
      }
      if (first_accessed.contains(to)) {
        removed_edge.emplace(std::make_pair(now, to));
        break;
      }

      visited.insert(to);
      q.push(to);
    }
  }

  if (!removed_edge) {
    // bug
    std::abort();
  }

  // Restore edges to forbidden vertices

  for (Vertex v : forbidden) {
    for (Vertex to : prev_iter.GetAdjacentVertices(v)) {
      if (removed_edge.value() == std::make_pair(v, to) ||
          removed_edge.value() == std::make_pair(to, v)) {
        continue;
      }

      iter.AddEdge(std::make_pair(v, to));
    }
  }

  return {std::move(iter), true, forbidden.size() == 1};
}

Graph FindMDST(const Graph& g) {
  if (g.GetVertices().size() == 0 || !g.IsLinked()) {
    return Graph();
  }

  Graph iter = GetAnyST(g);

  Vertex max_degree_vertex = iter.GetVertexOfMaximumDegree();
  uint64_t max_degree = iter.GetDegree(max_degree_vertex);

  for (uint64_t degree = max_degree; degree > 0;) {
    auto [next_iter, success, final_max_vertex] =
        Phase(g, std::move(iter), degree);

    if (!success) {
      return std::move(next_iter);
    }
    if (final_max_vertex) {
      --degree;
    }

    iter = std::move(next_iter);
  }

  return std::move(iter);
}

}  // namespace MDST