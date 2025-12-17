#ifndef __CLI_H__
#define __CLI_H__

#include <string>
#include <functional>
#include <vector>

namespace MinimalAudioEngine
{

/** @struct Command 
 *  @brief Structure to define a command line argument for the CLI parser
 */
struct Command
{
  std::string argument;
  std::string argument_short;
  std::string description;
  std::function<void(const char *arg)> action;

  Command(const std::string& arg, const std::string& arg_short, const std::string& desc, std::function<void(const char *arg)> act = nullptr)
    : argument(arg), argument_short(arg_short), description(desc), action(act) {}
};

typedef std::vector<Command> CommandList;

/** @class CLI 
 *  @brief Command Line Interface (CLI) parser
 */
class CLI
{
public:
  /** @brief CLI Constructor
   *  @param program_name Name of the program
   *  @param description Description of the program
   *  @param version Version of the program
   *  @param commands List of possible user-defined commands
   */
  CLI(const std::string &program_name,
      const std::string &description,
      const std::string &version,
      const CommandList &commands)
    : m_program_name(program_name), m_description(description), m_version(version), m_commands(commands)
    {
      // Add default commands for help and version
      m_commands.push_back(Command("--help", "-h", "Show help message", [this](const char *){
        this->help(nullptr);
      }));
      m_commands.push_back(Command("--version", "-v", "Show version information", [this](const char *){
        this->version(nullptr);
      }));
    }

  ~CLI() = default;

  /** @brief Parse command line arguments from CLI
   *  @param argc Argument count
   *  @param argv Argument vector
   */
  void parse_command_line_arguments(int argc, char *argv[]);

private:
  std::string m_program_name;
  std::string m_description;
  std::string m_version;
  CommandList m_commands;

  void help(const char *arg);
  void version(const char *arg);
};


} // namespace MinimalAudioEngine

#endif // __CLI_H__