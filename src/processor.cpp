#include "processor.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::vector;

Processor::Processor(){};

Processor::Processor(int number) { this->number_ = number; }

double Processor::Utilization() {
  vector<string> cpu_stats;
  if (this->number_ == -1) {
    cpu_stats = LinuxParser::CpuUtilization();
  } else if (this->number_ >= 0 && this->number_ <= 3) {
    cpu_stats = LinuxParser::CpuUtilization(std::to_string(this->number_));
  } else {
    throw std::invalid_argument("The CPU Core number does not exist.");
  }
  double usage = ComputeUtilization(cpu_stats);
  SetCpuStats(cpu_stats);
  return usage;
}

/*
 * This link describes the calculation
 * https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 */
float Processor::ComputeUtilization(vector<string> &stats) {
  // Calculate idle time
  auto prev_idle = prev_idle_ + prev_iowait_;
  auto idle = std::stol(stats[4]) + std::stol(stats[5]);

  // Calculate non-idle time
  auto prev_non_idle = prev_user_ + prev_nice_ + prev_system_ + prev_irq_ +
                       prev_soft_irq_ + prev_steal_;
  auto non_idle = std::stol(stats[1]) + std::stol(stats[2]) +
                  std::stol(stats[3]) + std::stol(stats[6]) +
                  std::stol(stats[7]) + std::stol(stats[8]);

  // Calculate total time
  auto prev_total = prev_idle + prev_non_idle;
  auto total = idle + non_idle;

  // Calculate differences
  auto total_diff = total - prev_total;
  auto idle_diff = idle - prev_idle;

  return (total_diff - idle_diff) / static_cast<float>(total_diff);
}

void Processor::SetCpuStats(vector<string> &stats) {
  this->prev_user_ = std::stof(stats[1]);
  this->prev_nice_ = std::stof(stats[2]);
  this->prev_system_ = std::stof(stats[3]);
  this->prev_idle_ = std::stof(stats[4]);
  this->prev_iowait_ = std::stof(stats[5]);
  this->prev_irq_ = std::stof(stats[6]);
  this->prev_soft_irq_ = std::stof(stats[7]);
  this->prev_steal_ = std::stof(stats[8]);
  this->prev_guest_ = std::stof(stats[9]);
  this->prev_guest_nice_ = std::stof(stats[10]);
}
