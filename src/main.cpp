#include "cli.h"
#include "logger.h"

using namespace GUI;

/** @brief Main function for the Digital Audio Workstation application.
 *  @return Exit status of the application (0 for success, non-zero for failure).
 */
int main()
{
  LOG_INFO("Embedded Audio Engine");
  LOG_INFO("---------------------");

  CommandLine cli;
  cli.run();

  LOG_INFO("Shutting down application...");

  return 0;
}
