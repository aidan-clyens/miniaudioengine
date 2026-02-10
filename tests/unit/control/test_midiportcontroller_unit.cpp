#include <gtest/gtest.h>

#include "midiportcontroller.h"
#include "logger.h"

using namespace miniaudioengine;
using namespace miniaudioengine::control;

class MidiPortControllerTest : public ::testing::Test
{
public:
  MidiPortControllerPtr get_midi_port_controller()
  {
    return p_midi_controller;
  }

private:
  MidiPortControllerPtr p_midi_controller{std::make_shared<MidiPortController>()};
};

TEST_F(MidiPortControllerTest, GetPorts)
{
  std::vector<MidiPort> ports = get_midi_port_controller()->get_ports();
  EXPECT_GE(ports.size(), 0);
  
  for (const auto &port : ports)
  {
    LOG_INFO("MIDI Port ", port.port_number, ": ", port.port_name);
    EXPECT_GE(port.port_number, 0);
    EXPECT_FALSE(port.port_name.empty());
  }
}

TEST_F(MidiPortControllerTest, OpenCloseInputPort)
{
  std::vector<MidiPort> ports = get_midi_port_controller()->get_ports();
  
  if (ports.empty())
  {
    LOG_WARNING("No MIDI input ports available for testing.");
    GTEST_SKIP() << "No MIDI input ports available.";
  }

  EXPECT_NO_THROW(get_midi_port_controller()->open_input_port(0));
  EXPECT_NO_THROW(get_midi_port_controller()->close_input_port());
}

TEST_F(MidiPortControllerTest, OpenInvalidPort)
{
  std::vector<MidiPort> ports = get_midi_port_controller()->get_ports();
  unsigned int invalid_port_number = ports.size(); // Out of range port number

  EXPECT_THROW(get_midi_port_controller()->open_input_port(invalid_port_number), std::out_of_range);
}

TEST_F(MidiPortControllerTest, ReopenPort)
{
  std::vector<MidiPort> ports = get_midi_port_controller()->get_ports();
  
  if (ports.empty())
  {
    LOG_WARNING("No MIDI input ports available for testing.");
    GTEST_SKIP() << "No MIDI input ports available.";
  }

  EXPECT_NO_THROW(get_midi_port_controller()->open_input_port(0));
  EXPECT_NO_THROW(get_midi_port_controller()->open_input_port(0)); // Reopen same port
  EXPECT_NO_THROW(get_midi_port_controller()->close_input_port());
}

TEST_F(MidiPortControllerTest, CloseWithoutOpen)
{
  EXPECT_NO_THROW(get_midi_port_controller()->close_input_port());
}