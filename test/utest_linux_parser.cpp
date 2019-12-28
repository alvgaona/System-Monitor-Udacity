#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "../include/linux_parser.h"

//--------------------------------//
//   Beginning LinuxParserTest Tests.
//--------------------------------//

class LinuxParserTest : public ::testing::Test {};

// Test the OperatingSystem function.
TEST_F(LinuxParserTest, TestLinuxParserOperatingSystem) {
  EXPECT_EQ(LinuxParser::OperatingSystem(), "Ubuntu 16.04.5 LTS");
}
