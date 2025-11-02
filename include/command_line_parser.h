#pragma once

#include <cxxopts.hpp>
#include <fmt/format.h>
#include <vector>

namespace cl
{
class CommandLineParser
{
private:
  cxxopts::Options options;
  std::vector<std::string> mandatory_options;

public:
  CommandLineParser(const std::string &name, const std::string &description) : options(name, description)
  {
    options.add_options()("h,help", "Print help");
  }

  template <typename T>
  void add_mandatory(const std::string &name, const std::string &description, const std::string &value_name = "")
  {
    std::string opt_name = name;
    if (!value_name.empty())
    {
      opt_name += "," + value_name;
    }
    options.add_options()(opt_name, description, cxxopts::value<T>());
    mandatory_options.push_back(name.substr(0, name.find(','))); // Store base name
  }

  template <typename T> void add_optional(const std::string &name, const std::string &description, T default_value)
  {
    options.add_options()(name, description, cxxopts::value<T>()->default_value(std::to_string(default_value)));
  }

  void add_optional_string(const std::string &name, const std::string &description,
                           const std::string &default_value = "")
  {
    options.add_options()(name, description, cxxopts::value<std::string>()->default_value(default_value));
  }

  cxxopts::ParseResult parse(int argc, const char *const argv[])
  {
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      fmt::println(options.help());
      exit(0);
    }

    check_mandatory_options(result);
    return result;
  }

private:
  void check_mandatory_options(const cxxopts::ParseResult &result)
  {
    bool missing_required = false;

    for (const auto &opt_name : mandatory_options)
    {
      if (!result.count(opt_name))
      {
        fmt::println(stderr, "Error: -- {} option is required", opt_name);
        missing_required = true;
      }
    }

    if (missing_required)
    {
      fmt::println(stderr, "\n {}", options.help());
      exit(1);
    }
  }
};
} // namespace cl
