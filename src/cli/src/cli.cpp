#include "cli.h"

#include <iostream>
#include <cstring>

using namespace MinimalAudioEngine;

void CLI::parse_command_line_arguments(int argc, char *argv[])
{
  if (argc < 2)
  {
    help(argv[0]);
    return;
  }

  m_program_name = argv[0];

  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    for (const auto &command : m_commands)
    {
      if (strcmp(arg.c_str(), command.argument.c_str()) == 0 ||
          strcmp(arg.c_str(), command.argument_short.c_str()) == 0)
      {
        if ((i + 1) < argc)
        {
          command.action(argv[i + 1]);
        }
        else
        {
          command.action(nullptr);
        }
        break;
      }
    }
  }
}

void CLI::help(const char *arg)
{
  (void)arg;

  std::cout << m_description << "\n";
  std::cout << "Usage: " << m_program_name << " [options]\n\n";
  std::cout << "Options:\n";
  for (const auto &command : m_commands)
  {
    std::cout << "  " << command.argument << ", " << command.argument_short << "\t" << command.description << "\n";
  }
  std::cout << std::endl;
  std::exit(0);
}

void CLI::version(const char *arg)
{
  (void)arg;
  std::cout << m_program_name << " Version " << m_version << "\n";
}