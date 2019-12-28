#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

/* TODO:
 *  Add Buffers. Use maps and remove and
 *  extract repeated code
 */
float LinuxParser::MemoryUtilization() {
  const string mem_total_keyword = "MemTotal";
  const string mem_free_keyword = "MemFree";

  float mem_total = -1;
  float mem_free = -1;

  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      if (line.compare(0, mem_total_keyword.size(), mem_total_keyword) == 0) {
        mem_total = std::stof(LinuxParser::ParseLine(line)[1]);
      }
      if (line.compare(0, mem_free_keyword.size(), mem_free_keyword) == 0) {
        mem_free = std::stof(LinuxParser::ParseLine(line)[1]);
      }

      if (mem_total >= 0 && mem_free >= 0) {
        break;
      }
    }
  }
  return (mem_total - mem_free) / mem_total;
}

/*
 *  Returns the CPU Uptime in seconds from /proc/uptime
 *  which is already in seconds.
 */
long int LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  std::getline(stream, line);
  return std::stol(LinuxParser::ParseLine(line)[0]);
}

long LinuxParser::Jiffies() { throw std::runtime_error("Not yet implemented"); }

// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) {
  throw std::runtime_error("Not yet implemented");
}

long LinuxParser::ActiveJiffies() {
  throw std::runtime_error("Not yet implemented");
}

long LinuxParser::IdleJiffies() {
  throw std::runtime_error("Not yet implemented");
}

int LinuxParser::NumberOfCpus() { return std::thread::hardware_concurrency(); }

vector<string> LinuxParser::CpuUtilization() {
  return ParseLineFrom(kStatFilename, "cpu");
}

vector<string> LinuxParser::CpuUtilization(const string core_number) {
  return ParseLineFrom(kStatFilename, "cpu" + core_number);
}

int LinuxParser::TotalProcesses() {
  return std::stoi(LinuxParser::ParseLineFrom(kStatFilename, "processes")[1]);
}

int LinuxParser::RunningProcesses() {
  return std::stoi(
      LinuxParser::ParseLineFrom(kStatFilename, "procs_running")[1]);
}

string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string line;
  std::getline(stream, line);
  return line;
}

string LinuxParser::Ram(int pid) {
  vector<string> line =
      ParseLineFrom(std::to_string(pid) + kStatusFilename, "VmSize:");
  return std::to_string(std::stof(line[1]) / 1024).substr(0, 3) + "MB";
}

string LinuxParser::Uid(int pid) {
  vector<string> line =
      ParseLineFrom(std::to_string(pid) + kStatusFilename, "Uid:");
  return line.at(1);
}

string LinuxParser::User(int pid) {
  auto uid = Uid(pid);
  return ParseUser(std::stoi(uid));
}

string LinuxParser::ParseUser(int uid) {
  string line;
  std::ifstream stream(kPasswordPath);
  auto name = "x:" + to_string(uid);
  while (std::getline(stream, line)) {
    if (line.find(name) != std::string::npos) {
      return line.substr(0, line.find(":"));
    }
  }
  throw std::runtime_error("User not found.");
}

/*
 * More info about it can be found in https://stackoverflow.com/a/16736599
 * and http://man7.org/linux/man-pages/man5/proc.5.html under /proc/[pid]/stat
 * section
 */
long int LinuxParser::UpTime(int pid) {
  auto line = StatFor(pid);

  auto utime = line.at(13);
  auto stime = line.at(14);
  auto ctime = line.at(15);
  auto cstime = line.at(16);

  return utime + stime + ctime + cstime;
}

string LinuxParser::StatFor(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  std::getline(stream, line);
  return line;
}

/*
 * Again more info about this can be found in
 * https://stackoverflow.com/a/16736599
 */
float LinuxParser::CpuUtilization(int pid) {
  auto line = StatFor(pid);
  auto pid_uptime = UpTime(pid) / sysconf(_SC_CLK_TCK);
  auto sys_uptime = UpTime();
  auto starttime = line.at(21);

  auto seconds = sys_uptime - (starttime / sysconf(_SC_CLK_TCK));

  return pid_uptime / static_cast<float>(seconds);
}

vector<string> LinuxParser::ParseLineFrom(const string filename,
                                          const string keyword) {
  string line;
  std::ifstream stream(kProcDirectory + filename);
  while (getline(stream, line)) {
    if (line.compare(0, keyword.size(), keyword) == 0) {
      return LinuxParser::ParseLine(line);
    }
  }

  vector<string> empty{};
  return empty;
}

vector<string> LinuxParser::ParseLine(string line) {
  std::istringstream iss(line);
  std::istream_iterator<string> segment(iss);
  std::istream_iterator<string> eos;
  vector<string> vec_data(segment, eos);

  return vec_data;
}
