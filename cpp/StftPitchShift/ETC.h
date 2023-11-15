#pragma once

#include <cmath>
#include <functional>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

template<typename T>
std::string join(const std::tuple<T, T>& values, const char delimiter)
{
  std::stringstream buffer;

  if (std::get<0>(values) != std::get<1>(values))
  {
    buffer << std::get<0>(values) << delimiter << std::get<1>(values);
  }
  else
  {
    buffer << std::get<0>(values);
  }

  return buffer.str();
}

template<typename T>
std::string join(const std::vector<T>& values, const char delimiter)
{
  std::stringstream buffer;

  for (size_t i = 0; i < values.size(); ++i)
  {
    if (i > 0)
    {
      buffer << delimiter;
    }

    buffer << values.at(i);
  }

  return buffer.str();
}

std::vector<std::string> split(const std::string& value, const char delimiter)
{
  std::vector<std::string> values;

  std::stringstream lines(value);
  std::string line;

  while (std::getline(lines, line, delimiter))
  {
    values.push_back(line);
  }

  return values;
}

bool semicent(const std::string& value)
{
  return (value.front() == '+') || (value.front() == '-') ||
         (value.front() == '0'  && (value.find('.') == std::string::npos));
}

double semitone(const std::string& value)
{
  std::regex regex("([+,-]?\\d+){1}([+,-]\\d+){0,1}");
  std::smatch matches;
  std::regex_match(value, matches, regex);
  std::string match = matches[1];
  return std::pow(2, std::stod(match) / 12);
}

double cent(const std::string& value)
{
  std::regex regex("([+,-]?\\d+){1}([+,-]\\d+){0,1}");
  std::smatch matches;
  std::regex_match(value, matches, regex);
  std::string match = (matches[2] == "") ? std::string("0") : matches[2];
  return std::pow(2, std::stod(match) / 1200);
}

size_t number(const std::string& value)
{
  if (value == "1k")
  {
    return 1 * 1024;
  }
  else if (value == "2k")
  {
    return 2 * 1024;
  }
  else if (value == "4k")
  {
    return 4 * 1024;
  }
  else
  {
    return std::stoi(value);
  }
}
