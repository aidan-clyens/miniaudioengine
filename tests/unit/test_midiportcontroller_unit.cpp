#include <gtest/gtest.h>

#include "midiportcontroller.h"
#include "logger.h"

using namespace MinimalAudioEngine;
using namespace MinimalAudioEngine::Control;

TEST(MidiPortControllerTest, GetPorts)
{
  std::vector<MidiPort> ports = MidiPortController::instance().get_ports();
  EXPECT_GE(ports.size(), 0);
  
  for (const auto &port : ports)
  {
    LOG_INFO("MIDI Port ", port.port_number, ": ", port.port_name);
    EXPECT_GE(port.port_number, 0);
    EXPECT_FALSE(port.port_name.empty());
  }
}

TEST(MidiPortControllerTest, OpenCloseInputPort)
{
  std::vector<MidiPort> ports = MidiPortController::instance().get_ports();
  
  if (ports.empty())
  {
    LOG_WARNING("No MIDI input ports available for testing.");
    GTEST_SKIP() << "No MIDI input ports available.";
  }

  EXPECT_NO_THROW(MidiPortController::instance().open_input_port(0));
  EXPECT_NO_THROW(MidiPortController::instance().close_input_port());
}

TEST(MidiPortControllerTest, OpenInvalidPort)
{
  std::vector<MidiPort> ports = MidiPortController::instance().get_ports();
  unsigned int invalid_port_number = ports.size(); // Out of range port number

  EXPECT_THROW(MidiPortController::instance().open_input_port(invalid_port_number), std::out_of_range);
}

TEST(MidiPortControllerTest, ReopenPort)
{
  std::vector<MidiPort> ports = MidiPortController::instance().get_ports();
  
  if (ports.empty())
  {
    LOG_WARNING("No MIDI input ports available for testing.");
    GTEST_SKIP() << "No MIDI input ports available.";
  }

  EXPECT_NO_THROW(MidiPortController::instance().open_input_port(0));
  EXPECT_NO_THROW(MidiPortController::instance().open_input_port(0)); // Reopen same port
  EXPECT_NO_THROW(MidiPortController::instance().close_input_port());
}

TEST(MidiPortControllerTest, CloseWithoutOpen)
{
  EXPECT_NO_THROW(MidiPortController::instance().close_input_port());
}