#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  Processor();
  Processor(int number);
  double Utilization();

 private:
  int number_{-1};
  float prev_user_{0};
  float prev_nice_{0};
  float prev_system_{0};
  float prev_idle_{0};
  float prev_iowait_{0};
  float prev_irq_{0};
  float prev_soft_irq_{0};
  float prev_steal_{0};
  float prev_guest_{0};
  float prev_guest_nice_{0};

  float ComputeUtilization(std::vector<std::string> &stats);
  void SetCpuStats(std::vector<std::string> &stats);
};

#endif
