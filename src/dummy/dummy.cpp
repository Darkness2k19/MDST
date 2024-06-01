#include "dummy.hpp"

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "utility/graph.hpp"

namespace MDSTDummy {

const char kMaxEdgeAmount = 28;
const char kMaxVertexAmount = kMaxEdgeAmount + 1;
char queue[kMaxVertexAmount];
char visited[kMaxVertexAmount];
MDST::Edge edges[kMaxEdgeAmount];
uint64_t g[kMaxVertexAmount];

MDST::Graph FindMDST(const MDST::Graph& input) {
  if (!input.IsLinked()) {
    return MDST::Graph();
  }

  int n = input.GetVertices().size();
  if (n == 1) {
    return input;
  }

  int m = input.GetEdges().size();

  if (m > kMaxEdgeAmount) {
    throw std::logic_error("Dummy algo cannot work with so many edges");
  }

  std::unordered_map<MDST::Vertex, char> to_my;
  std::vector<MDST::Vertex> to_initial(input.GetVertices().size());

  int cur = 0;
  for (MDST::Vertex v : input.GetVertices()) {
    to_my[v] = cur;
    to_initial[cur++] = v;
  }

  int edges_cnt = 0;
  for (MDST::Edge e : input.GetEdges()) {
    edges[edges_cnt++] = std::make_pair(to_my[e.first], to_my[e.second]);
  }

  uint64_t ans = 0;
  int max_degree = n;

  for (uint64_t mask = (1 << (n - 1)) - 1; mask < (1 << m); ++mask) {
    if (__builtin_popcountll(mask) != n - 1) {
      continue;
    }
    memset(g, 0, sizeof(uint64_t) * n);

    int count = 0;
    int e = 0;
    uint64_t cur = 1;
    for (; e < m; ++e, cur <<= 1) {
      if (!(mask & cur)) {
        continue;
      }

      g[edges[e].first] |= (1 << edges[e].second);
      g[edges[e].second] |= (1 << edges[e].first);
    }

    int degree = __builtin_popcountll(g[0]);
    for (int v = 1; v < n; ++v) {
      int deg = __builtin_popcountll(g[v]);
      if (deg > degree) {
        degree = deg;
      }
    }
    if (degree >= max_degree) {
      continue;
    }

    memset(queue, 0, kMaxVertexAmount);
    memset(visited, 0, kMaxVertexAmount);

    int qbegin = 0;
    int qend = 1;
    visited[0] = 1;
    int vis_cnt = 1;

    while (qend != qbegin && vis_cnt < n) {
      char now = queue[qbegin++];
      if (qbegin == kMaxVertexAmount) {
        qbegin = 0;
      }

      int v = 0;
      uint64_t vertex_mask = 1;
      for (; v < n && vis_cnt < n; ++v, vertex_mask <<= 1) {
        if (v == now || visited[v] || !(g[now] & vertex_mask)) {
          continue;
        }

        ++vis_cnt;
        visited[v] = 1;
        queue[qend++] = v;
        if (qend == kMaxVertexAmount) {
          qend = 0;
        }
      }
    }
    if (vis_cnt == n) {
      max_degree = degree;
      ans = mask;
    }
  }

  MDST::Graph result;
  int e = 0;
  uint64_t edge_mask = 1;
  for (; e < m; ++e, edge_mask <<= 1) {
    if (!(ans & edge_mask)) {
      continue;
    }

    result.AddEdge(std::make_pair(to_initial[edges[e].first],
                                  to_initial[edges[e].second]));
  }
  return result;
}

}  // namespace MDSTDummy