#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>

class Processor {
 public:
  Processor();
  Processor(int number);
  float Utilization();

 private:
  int number_{0};
  long prev_user_{0};
  long prev_nice_{0};
  long prev_system_{0};
  long prev_idle_{0};
  long prev_iowait_{0};
  long prev_irq_{0};
  long prev_soft_irq_{0};
  long prev_steal_{0};
  long prev_guest_{0};
  long prev_guest_nice_{0};
  
  float ComputeUtilization(std::vector<std::string> &stats);
  void SetCpuStats(std::vector<std::string> stats);
};

#endif