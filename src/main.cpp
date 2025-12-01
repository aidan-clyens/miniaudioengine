#include "coreengine.h"
#include "logger.h"

#include "devicemanager.h"

#include <iostream>
#include <csignal>

using namespace Core;

static bool app_running = true;

/** @brief Signal handler for graceful shutdown on SIGINT (Ctrl+C).
 *  This function sets the app_running flag to false, allowing the main loop to exit cleanly.
 *
 *  @param signum The signal number (not used here).
 */
void signal_handler(int signum) 
{
  app_running = false;
}

/** @brief Main function for the Digital Audio Workstation application.
 *  @return Exit status of the application (0 for success, non-zero for failure).
 */
int main()
{
  LOG_INFO("Embedded Audio Engine");
  LOG_INFO("---------------------");

  std::signal(SIGINT, signal_handler);

  CoreEngine::instance().start_thread();

  std::cout << "Type 'help' for a list of commands.\n";

  std::string cmd;
  while (app_running)
  {
    std::cout << "> ";
    std::getline(std::cin, cmd);
    
    if (cmd == "quit" || cmd == "q")
    {
      app_running = false;
    }
    else if (cmd  == "midi-devices")
    {
      auto devices = Devices::DeviceManager::instance().get_midi_devices();
      for (const auto& device : devices)
      {
        std::cout << "MIDI Device ID: " << device.id << ", Name: " << device.name << "\n";
      }
    }
    else if (cmd == "audio-devices")
    {
      auto devices = Devices::DeviceManager::instance().get_audio_devices();
      for (const auto& device : devices)
      {
        std::cout << "Audio Device ID: " << device.id << ", Name: " << device.name << "\n";
      }
    }
    else if (cmd == "help")
    {
      std::cout << "Available commands:\n";
      std::cout << "  help          - Show this help message\n";
      std::cout << "  midi-devices  - List available MIDI devices\n";
      std::cout << "  audio-devices - List available Audio devices\n";
      std::cout << "  quit, q       - Quit the application\n";
    }
    else if (cmd.empty())
    {
      // Ignore empty commands
    }
    else
    {
      std::cout << "Unknown command: " << cmd << "\n";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  LOG_INFO("Shutting down application...");
  CoreEngine::instance().push_message({CoreEngineMessage::eType::Shutdown, "SIGINT received"});
  CoreEngine::instance().stop_thread();

  return 0;
}
