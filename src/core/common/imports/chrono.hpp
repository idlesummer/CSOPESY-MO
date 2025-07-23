#pragma once
#include <chrono>
#include <ctime>
#include <optional>


using std::chrono::duration_cast;

// Aliases for duration types
using ms  = std::chrono::milliseconds;
using ns  = std::chrono::nanoseconds;
using sec = std::chrono::seconds;

// Clock and time point
using duration = std::chrono::system_clock::duration;
using Clock = std::chrono::system_clock;
using Time = std::chrono::time_point<std::chrono::system_clock>;
using TimeStruct = std::tm;

// Literal suffixes for values like 10ms, 1s, 250ns
using std::chrono_literals::operator""ms;
using std::chrono_literals::operator""ns;
using std::chrono_literals::operator""s;

/** Returns a formatted timestamp string using current local time. */
auto timestamp(std::optional<Time> tp=std::nullopt, const std::string& fmt="%m/%d/%Y %I:%M:%S%p") -> std::string {
  auto time = Clock::to_time_t(tp.value_or(Clock::now()));
  auto time_struct = TimeStruct();
  localtime_s(&time_struct, &time);

  char buffer[32];
  strftime(buffer, sizeof(buffer), fmt.c_str(), &time_struct);
  return buffer;
}
