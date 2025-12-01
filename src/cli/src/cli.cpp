#include "cli.h"

#include "devicemanager.h"
#include "trackmanager.h"
#include "coreengine.h"
#include "logger.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <csignal>

using namespace GUI;

bool CLI::m_app_running = false;

std::map<eCLICommand, std::function<void()>> CLI::m_cmd_function_map =
{
  {eCLICommand::Quit, []() { CLI::m_app_running = false; }},
  {eCLICommand::Quit,    []() { CLI::m_app_running = false; }},
  {eCLICommand::Help, []() {
    std::cout << "Available commands:\n";
    std::cout << "  help, h       - Show this help message\n";
    std::cout << "  midi-devices  - List available MIDI devices\n";
    std::cout << "  audio-devices - List available Audio devices\n";
    std::cout << "  list-tracks   - List all tracks\n";
    std::cout << "  add-track     - Add a new track\n";
    std::cout << "  quit, q       - Quit the application\n";
  }},
  {eCLICommand::Help, []() {
    std::cout << "Available commands:\n";
    std::cout << "  help, h       - Show this help message\n";
    std::cout << "  midi-devices  - List available MIDI devices\n";
    std::cout << "  audio-devices - List available Audio devices\n";
    std::cout << "  list-tracks   - List all tracks\n";
    std::cout << "  add-track     - Add a new track\n";
    std::cout << "  quit, q       - Quit the application\n";
  }},
  {eCLICommand::ListMidiDevices, []() {
    auto devices = Devices::DeviceManager::instance().get_midi_devices();
    for (const auto &device : devices)
    {
      std::cout << "MIDI Device ID: " << device.id << ", Name: " << device.name << "\n";
    }
  }},
  {eCLICommand::ListAudioDevices, []() {
    auto devices = Devices::DeviceManager::instance().get_audio_devices();
    for (const auto &device : devices)
    {
      std::cout << "Audio Device ID: " << device.id << ", Name: " << device.name << "\n";
    }
  }},
  {eCLICommand::ListTracks, []() {
    size_t track_count = Tracks::TrackManager::instance().get_track_count();
    std::cout << "Total tracks: " << track_count << "\n";
    for (size_t i = 0; i < track_count; ++i)
    {
      auto track = Tracks::TrackManager::instance().get_track(i);
      std::cout << "Track " << i << ": " << track->to_string() << "\n";
    }
  }},
  {eCLICommand::AddTrack, []() {
    Tracks::TrackManager::instance().add_track();
    std::cout << "Track added. Total tracks: " << Tracks::TrackManager::instance().get_track_count() << "\n";
  }}
};

CLI::CLI()
{
  m_app_running = true;
  std::signal(SIGINT, CLI::handle_shutdown_signal);
}

/** @brief Parses a command string and returns the corresponding eCLICommand enum value.
 *
 *  @param cmd The command string to parse.
 *  @return The corresponding eCLICommand enum value.
 */
eCLICommand CLI::parse_command(const std::string &cmd)
{
  if (cmd == "help" || cmd == "h")
    return eCLICommand::Help;
  else if (cmd == "quit" || cmd == "q")
    return eCLICommand::Quit;
  else if (cmd == "midi-devices")
    return eCLICommand::ListMidiDevices;
  else if (cmd == "audio-devices")
    return eCLICommand::ListAudioDevices;
  else if (cmd == "list-tracks")
    return eCLICommand::ListTracks;
  else if (cmd == "add-track")
    return eCLICommand::AddTrack;
  else
    return eCLICommand::Unknown;
}

/** @brief Signal handler for graceful shutdown on SIGINT (Ctrl+C).
 *  This function sets the app_running flag to false, allowing the main loop to exit cleanly.
 *
 *  @param signum The signal number (not used here).
 */
void CLI::handle_shutdown_signal(int signum)
{
  m_app_running = false;
}

/** @brief Runs the command-line interface, processing user input and executing commands.
 */
void CLI::run()
{
  std::cout << "Type 'help' for a list of commands.\n";

  std::string command_str;
  while (m_app_running)
  {
    std::cout << CLI_PROMPT;
    std::getline(std::cin, command_str);

    eCLICommand command = parse_command(command_str);

    auto it = m_cmd_function_map.find(command);
    if (it != m_cmd_function_map.end())
    {
      it->second();  // Execute the command function
    }
    else
    {
      std::cout << "Unknown command: " << command_str << "\n";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}