#include <gtest/gtest.h>

#include "midicontroller.h"
#include "midiadapter.h"
#include "logger.h"

using namespace miniaudioengine;
using namespace miniaudioengine::midi;

class MidiControllerTest : public ::testing::Test
{
public:
  MidiControllerPtr get_midi_controller()
  {
    return p_midi_controller;
  }

private:
  MidiControllerPtr p_midi_controller{std::make_shared<MidiController>()};
};

TEST_F(MidiControllerTest, GetPorts)
{
  std::vector<adapter::MidiPort> ports = get_midi_controller()->get_ports();
  EXPECT_GE(ports.size(), 0);

  for (const auto &port : ports)
  {
    LOG_INFO("MIDI Port ", port.port_number, ": ", port.port_name);
    EXPECT_GE(port.port_number, 0);
    EXPECT_FALSE(port.port_name.empty());
  }
}

TEST_F(MidiControllerTest, OpenCloseInputPort)
{
  std::vector<adapter::MidiPort> ports = get_midi_controller()->get_ports();

  if (ports.empty())
  {
    LOG_WARNING("No MIDI input ports available for testing.");
    GTEST_SKIP() << "No MIDI input ports available.";
  }

  EXPECT_NO_THROW(get_midi_controller()->open_input_port(0));
  EXPECT_NO_THROW(get_midi_controller()->close_input_port());
}

TEST_F(MidiControllerTest, OpenInvalidPort)
{
  std::vector<adapter::MidiPort> ports = get_midi_controller()->get_ports();
  unsigned int invalid_port_number = ports.size(); // Out of range port number

  EXPECT_THROW(get_midi_controller()->open_input_port(invalid_port_number), std::out_of_range);
}

TEST_F(MidiControllerTest, ReopenPort)
{
  std::vector<adapter::MidiPort> ports = get_midi_controller()->get_ports();

  if (ports.empty())
  {
    LOG_WARNING("No MIDI input ports available for testing.");
    GTEST_SKIP() << "No MIDI input ports available.";
  }

  EXPECT_NO_THROW(get_midi_controller()->open_input_port(0));
  EXPECT_NO_THROW(get_midi_controller()->open_input_port(0)); // Reopen same port
  EXPECT_NO_THROW(get_midi_controller()->close_input_port());
}

TEST_F(MidiControllerTest, CloseWithoutOpen)
{
  EXPECT_NO_THROW(get_midi_controller()->close_input_port());
}
