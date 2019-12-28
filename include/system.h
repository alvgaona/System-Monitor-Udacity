#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

/*
 * Modify this class to LinuxSystem and create
 * an abstract base class called System with
 * pure virtual members to be overriden by
 * concrete classes (e. g. LinuxSystem, BsdSystem, FreeBsdSystem)
 * to make this system monitor portable to different UNIX based OS.
 */
class System {
 public:
  System();
  Processor& Cpu();
  Processor& Cpu(int number);
  std::vector<Process>& Processes();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  std::string Kernel();
  std::string OperatingSystem();

 private:
  Processor cpu_;
  std::vector<Processor> cpus_ = {};
  std::vector<Process> processes_ = {};
};

#endif