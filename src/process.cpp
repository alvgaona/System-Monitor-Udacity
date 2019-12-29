#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using namespace std;

Process::Process(int pid) {
  this->pid_ = pid;
  this->user_ = LinuxParser::User(pid);
  this->command_ = LinuxParser::Command(pid);
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return LinuxParser::CpuUtilization(pid_); }

string Process::Command() {
 const int MAX_LENGTH = 40;
 if (command_.length() > MAX_LENGTH) {
  auto shortened_command = command_.substr(0, MAX_LENGTH);
  return shortened_command + "...";
 }
 return command_;
}

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return user_; }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return this->cpu_utilization_ < a.cpu_utilization_;
}
