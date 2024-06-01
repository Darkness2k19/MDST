#include <gtest/gtest.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>

#include <utility/reader.hpp>

class CORRECTNESS : public ::testing::TestWithParam<size_t> {
protected:
  std::ifstream in;
  std::ifstream out;

  MDST::Reader parse_in;
  MDST::Reader parse_out;

protected:
  void SetUp() override {
    in.open(std::filesystem::path(__FILE__).parent_path() / "data" /
            (std::to_string(GetParam()) + ".in"));
    out.open(std::filesystem::path(__FILE__).parent_path() / "data" /
             (std::to_string(GetParam()) + ".out"));

    parse_in = MDST::Reader(in);
    parse_out = MDST::Reader(out);
  }

  void TearDown() override {
    in.close();
    out.close();
  }
};
