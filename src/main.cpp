#include "audioengine.h"
#include "midiengine.h"
#include "trackmanager.h"
#include "track.h"
#include "messagequeue.h"
#include "logger.h"

#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <vector>

using namespace Audio;
using namespace Midi;
using namespace Tracks;

static bool app_running = false;

/** @enum eAppCommand 
 *  @brief Application commands for main event loop
 */
typedef enum class eAppCommand
{
  GetMidiPorts,
  Quit,
} eAppCommand;

/** @class Application
 *  @brief The main Audio Engine Platform application
 */
class Application
{
public:
  Application()
  {
    AudioEngine::instance().start_thread();
    MidiEngine::instance().start_thread();
  }

  ~Application()
  {
    MidiEngine::instance().stop_thread();
    AudioEngine::instance().stop_thread();
  }

  void post_command(eAppCommand command)
  {
    m_message_queue.push(command);
  }

  void run()
  {
    size_t track_index = TrackManager::instance().add_track();
    auto track = TrackManager::instance().get_track(track_index);

    // Attach track as observer to both engines
    MidiEngine::instance().attach(track);

    while (app_running)
    {
      auto command = m_message_queue.try_pop();
      if (command.has_value())
      {
        switch (command.value())
        {
          case eAppCommand::GetMidiPorts:
            {
              auto ports = MidiEngine::instance().get_ports();
              LOG_INFO("MIDI Input Ports:");
              for (const auto& port : ports)
              {
                LOG_INFO("Port ", port.port_number, ": ", port.port_name);
              }
            }
            break;
          case eAppCommand::Quit:
            app_running = false;
            break;
          default:
            break;
        }
      }

      track->handle_midi_message();

      // Wait for the signal handler to set app_running to false
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    MidiEngine::instance().detach(track);
    TrackManager::instance().clear_tracks();
  }

private:
  // Add message queue for application from UI thread
  MessageQueue<eAppCommand> m_message_queue;
};

/** @brief Signal handler for graceful shutdown on SIGINT (Ctrl+C).
 *  This function sets the app_running flag to false, allowing the main loop to exit cleanly.
 *
 *  @param signum The signal number (not used here).
 */
void signal_handler(int signum) 
{
  std::cout << "\nExiting.\n";
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
  app_running = true;

  Application app;
  app.run();

  return 0;
}
