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
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os;
  string version;
  string kernel;
  string line;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  filestream.close();
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
        pids.emplace_back(pid);
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
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
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
  filestream.close();
  return (mem_total - mem_free) / mem_total;
}

/*
 *  Returns the CPU Uptime in seconds from /proc/uptime
 *  which is already in seconds.
 */
long int LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string line;
  std::getline(filestream, line);
  filestream.close();
  return std::stol(LinuxParser::ParseLine(line)[0]);
}

long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

long LinuxParser::ActiveJiffies(int pid) {
  vector<string> stat_line = ParseLine(StatFor(pid));

  long jiffies{0};
  if (stat_line.size() > 21) {
    long user = std::stol(stat_line[13]);
    long kernel = std::stol(stat_line[14]);
    long children_user = std::stol(stat_line[15]);
    long children_kernel = std::stol(stat_line[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }
  return jiffies;
}

long LinuxParser::ActiveJiffies() {
  vector<string> time = CpuUtilization();
  return (std::stol(time[CPUStates::kUser_]) + std::stol(time[CPUStates::kNice_]) +
          std::stol(time[CPUStates::kSystem_]) + std::stol(time[CPUStates::kIRQ_]) +
          std::stol(time[CPUStates::kSoftIRQ_]) + std::stol(time[CPUStates::kSteal_]) +
          std::stol(time[CPUStates::kGuest_]) + std::stol(time[CPUStates::kGuestNice_]));
}

long LinuxParser::IdleJiffies() {
  vector<string> time = CpuUtilization();
  return (std::stol(time[CPUStates::kIdle_]) + std::stol(time[CPUStates::kIOwait_]));
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
  return std::stoi(LinuxParser::ParseLineFrom(kStatFilename, "procs_running")[1]);
}

string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string line;
  std::getline(filestream, line);
  filestream.close();
  return line;
}

/*
* In this function uses VmData rather than VmSize since
* VmData gives the exact Physical RAM Size whereas VmSize
* gives the virtual memory used.
*/
string LinuxParser::Ram(int pid) {
  vector<string> line =
      ParseLineFrom(std::to_string(pid) + kStatusFilename, "VmData:");
  return std::to_string(std::stof(line[1]) / 1024).substr(0, 6);
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
  std::ifstream filestream(kPasswordPath);
  auto name = "x:" + to_string(uid);
  while (std::getline(filestream, line)) {
    if (line.find(name) != std::string::npos) {
      filestream.close();
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
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  std::getline(filestream, line);
  filestream.close();
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
  std::ifstream filestream(kProcDirectory + filename);
  while (getline(filestream, line)) {
    if (line.compare(0, keyword.size(), keyword) == 0) {
      return LinuxParser::ParseLine(line);
    }
  }
  filestream.close();

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
