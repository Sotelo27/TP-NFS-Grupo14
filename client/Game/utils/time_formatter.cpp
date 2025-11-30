#include "time_formatter.h"

#include <iomanip>
#include <sstream>

std::string TimeFormatter::format_time(int time_seconds) {
    time_seconds = (time_seconds < 0) ? 0 : time_seconds;

    int minutes = time_seconds / 60;
    int seconds = time_seconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << std::abs(minutes) << ":" << std::setfill('0')
       << std::setw(2) << std::abs(seconds);
    return ss.str();
}
