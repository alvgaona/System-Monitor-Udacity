#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  this->pid_ = pid;
  this->uptime_ = LinuxParser::UpTime(pid);
  this->cpu_utilization_ = LinuxParser::CpuUtilization(pid);
  this->user_ = LinuxParser::User(pid);
  this->command_ = LinuxParser::Command(pid);
  this->ram_ = LinuxParser::Ram(pid);
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpu_utilization_; }

string Process::Command() { return command_; }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

// TODO: Overload < to compare CPU Utilization
bool Process::operator<(Process const& a) const {
  return true;
}

