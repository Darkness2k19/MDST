#pragma once

#include <istream>

#include "graph.hpp"

namespace MDST {

class Reader {
public:
  Reader() = default;
  Reader(std::istream& in);

  Graph Read();

private:
  std::istream* in_ = nullptr;
};

}  // namespace MDST