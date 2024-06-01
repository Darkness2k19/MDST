#include "writer.hpp"

#include <ostream>
#include <stdexcept>
#include <utility/graph.hpp>

namespace MDST {

Writer::Writer(std::ostream& out)
    : out_(&out) {}

void Writer::Write(const Graph& g) const {
  if (out_ == nullptr) {
    throw std::runtime_error("no stream to read from");
  }

  std::ostream& out = *out_;

  uint64_t vertices = g.GetVertices().size(), edges = g.GetEdges().size();
  out << vertices << ' ' << edges << ' ';
  
#ifdef STATS
  if (vertices != 0){
    out << g.GetDegree(g.GetVertexOfMaximumDegree());
  } else {
    out << 0;
  }
#endif

  out << '\n';
  for (Vertex v : g.GetVertices()) {
    out << v << ' ';
  }
  out << '\n';

  for (Edge e : g.GetEdges()) {
    out << e.first << ' ' << e.second << '\n';
  }
}

}  // namespace MDST