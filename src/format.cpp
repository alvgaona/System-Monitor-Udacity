#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long int elapsed_time) {
  int hours = (elapsed_time / seconds_in_hour) % hours_in_day;
  int minutes = (elapsed_time % seconds_in_hour) / seconds_in_minute;
  int seconds = elapsed_time % seconds_in_minute;

  std::string hh =
      hours >= 10 ? std::to_string(hours) : "0" + std::to_string(hours);
  std::string mm =
      minutes >= 10 ? std::to_string(minutes) : "0" + std::to_string(minutes);
  std::string ss =
      seconds >= 10 ? std::to_string(seconds) : "0" + std::to_string(seconds);

  return hh + ":" + mm + ":" + ss;
}