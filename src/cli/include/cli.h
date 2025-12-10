#ifndef __CLI_H__
#define __CLI_H__

#include <string>
#include <memory>
#include <vector>

#include <replxx.hxx>

#include "coreengine.h"

namespace CLI { class App; }

namespace GUI
{

constexpr const char *CLI_WELCOME_MESSAGE = "Welcome to the Minimal Audio Engine CommandLine! Type 'help' for a list of commands.\n";
constexpr const char *CLI_PROMPT = "> ";

/** @class CommandLine
 *  @brief Command-Line Interface for interacting with the application
 */
class CommandLine
{
public:
  CommandLine();
  ~CommandLine();

  void run();
  void stop();

private:
  void setup_commands();
  void setup_autocomplete();
  
  // Autocomplete callback
  replxx::Replxx::completions_t completion_callback(std::string const& input, int& contextLen);

  // Command handlers
  void cmd_list_midi_devices();
  void cmd_list_audio_devices();
  void cmd_list_tracks();
  void cmd_add_track();
  void cmd_get_track(unsigned int track_id);
  void cmd_add_track_audio_input_device(unsigned int track_id, unsigned int device_id);
  void cmd_add_track_audio_input_file(unsigned int track_id, const std::string& file_path);
  void cmd_add_track_audio_output_device(unsigned int track_id, unsigned int device_id);
  void cmd_play_track(unsigned int track_id);
  void cmd_stop_track(unsigned int track_id);
  
  void show_help();

  static void handle_shutdown_signal(int signum);

  Core::CoreEngine m_engine;
  std::unique_ptr<::CLI::App> m_cli_app;
  std::unique_ptr<replxx::Replxx> m_replxx;

  // Command argument storage
  unsigned int m_track_id;
  unsigned int m_input_device_id;
  unsigned int m_output_device_id;
  std::string m_input_file_path;

  static bool m_app_running;
};

}; // namespace GUI

#endif // __CLI_H__