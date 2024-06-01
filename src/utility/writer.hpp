#include <ostream>
#include <utility/graph.hpp>

namespace MDST {

class Writer {
public:
  Writer() = default;
  Writer(std::ostream& out);

  void Write(const Graph&) const;

private:
  std::ostream* out_ = nullptr;
};

}