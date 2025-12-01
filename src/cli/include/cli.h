#ifndef __CLI_H__
#define __CLI_H__

#include <functional>
#include <map>
#include <string>

namespace GUI
{

constexpr const char *CLI_PROMPT = "> ";

/** @enum eCLICommand
 *  @brief Supported CLI commands
 */
enum class eCLICommand
{
  Help,
  Quit,
  ListMidiDevices,
  ListAudioDevices,
  ListTracks,
  AddTrack,
  Unknown
};

/** @class CLI
 *  @brief Command-Line Interface for interacting with the application
 */
class CLI
{
public:
  CLI();
  ~CLI() = default;

  void run();
  void stop() { m_app_running = false; }

private:
  eCLICommand parse_command(const std::string &cmd);
  
  static void handle_shutdown_signal(int signum);

  static bool m_app_running;
  static std::map<eCLICommand, std::function<void()>> m_cmd_function_map;
};

}; // namespace GUI

#endif // __CLI_H__