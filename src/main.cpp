#include <iostream>

#include <algo/mdst.hpp>
#include <dummy/dummy.hpp>
#include <utility/graph.hpp>
#include <utility/reader.hpp>
#include <utility/writer.hpp>

int main() {
  MDST::Reader reader(std::cin);

  MDST::Graph g = reader.Read();

#ifdef DUMMY
  MDST::Graph result = MDSTDummy::FindMDST(g);
#else
  MDST::Graph result = MDST::FindMDST(g);
#endif

  if (result.GetVertices().size() == 0) {
    std::cout << "no mst in graph";
    return 0;
  }

  MDST::Writer writer(std::cout);

  writer.Write(result);

  return 0;
}
