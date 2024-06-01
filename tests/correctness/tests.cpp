#include "tests.hpp"

#include <gtest/gtest.h>

#include <algo/mdst.hpp>
#include <dummy/dummy.hpp>
#include <cstring>
#include <utility/graph.hpp>

TEST_P(CORRECTNESS, Test) {
  MDST::Graph graph_in = parse_in.Read();
  MDST::Graph mdst_out = parse_out.Read();
#ifndef DUMMY
  MDST::Graph mdst = MDST::FindMDST(graph_in);
#else
  MDST::Graph mdst = MDSTDummy::FindMDST(graph_in);
#endif
  if (mdst_out.GetVertices().size() == 0) {
    EXPECT_EQ(mdst.GetVertices().size(), 0);
  } else {
    EXPECT_GT(mdst.GetVertices().size(), 0);
    EXPECT_TRUE(mdst.IsLinked());

    MDST::Vertex max_out = mdst_out.GetVertexOfMaximumDegree();
    MDST::Vertex max_mdst = mdst.GetVertexOfMaximumDegree();

#ifndef DUMMY
    EXPECT_GE(mdst.GetDegree(max_mdst), mdst_out.GetDegree(max_out));
    EXPECT_LE(mdst.GetDegree(max_mdst), mdst_out.GetDegree(max_out) + 1);
#else
    EXPECT_EQ(mdst.GetDegree(max_mdst), mdst_out.GetDegree(max_out));
#endif
  }
}

INSTANTIATE_TEST_SUITE_P(, CORRECTNESS, ::testing::Range(1ul, 8ul));