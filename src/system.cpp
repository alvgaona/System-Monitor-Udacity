#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using namespace std;

System::System() {
  int num_of_cpus = LinuxParser::NumberOfCpus();
  for (int i = 0; i < num_of_cpus; i++) {
    Processor processor(i);
    cpus_.emplace_back(processor);
  }
}

vector<Process>& System::Processes() {
  if (processes_.size() > 0) {
    processes_.clear();
  }
  
  vector<int> pids(LinuxParser::Pids());
  for (auto& pid : pids) {
    Process process(pid);
    processes_.emplace_back(process);
  }
  return processes_;
}

Processor& System::Cpu() { return cpu_; }

Processor& System::Cpu(int number) { return cpus_[number]; }

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long System::UpTime() { return LinuxParser::UpTime(); }