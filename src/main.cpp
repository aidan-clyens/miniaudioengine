#include "coreengine.h"
#include "cli.h"
#include "logger.h"

using namespace Core;
using namespace GUI;

static bool app_running = true;

/** @brief Main function for the Digital Audio Workstation application.
 *  @return Exit status of the application (0 for success, non-zero for failure).
 */
int main()
{
  LOG_INFO("Embedded Audio Engine");
  LOG_INFO("---------------------");

  CoreEngine engine;
  CLI cli;

  engine.start_thread();
  cli.run();

  LOG_INFO("Shutting down application...");
  engine.push_message({CoreEngineMessage::eType::Shutdown, "SIGINT received"});
  engine.stop_thread();

  return 0;
}
