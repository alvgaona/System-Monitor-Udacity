#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>
#include "../include/linux_parser.h"

//--------------------------------//
//   Beginning RoutePlanner Tests.
//--------------------------------//

class LinuxParserTest : public ::testing::Test {
 protected:


  // Construct start_node and end_node as in the model.
  float start_x = 0.1;
  float start_y = 0.1;
  float end_x = 0.9;
  float end_y = 0.9;
  RouteModel::Node* start_node = &model.FindClosestNode(start_x, start_y);
  RouteModel::Node* end_node = &model.FindClosestNode(end_x, end_y);

  // Construct another node in the middle of the map for testing.
  float mid_x = 0.5;
  float mid_y = 0.5;
  RouteModel::Node* mid_node = &model.FindClosestNode(mid_x, mid_y);
};

// Test the OperatingSystem function.
TEST_F(LinuxParserTest, TestLinuxParserOperatingSystem) {
  EXPECT_EQ(LinuxParser::OperatingSystem(), "Linux");
}
