#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);
static const int seconds_in_hour = 3600;
static const int seconds_in_minute = 60;
static const int hours_in_day = 24;
};  // namespace Format

#endif