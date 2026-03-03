#ifndef __TRACK_MIDI_DATA_PLANE_H__
#define __TRACK_MIDI_DATA_PLANE_H__

#include <memory>
#include <atomic>
#include <string>

#include "dataplane.h"

namespace miniaudioengine::midi
{
// Forward declaration
struct MidiMessage;
} // namespace miniaudioengine::midi

namespace miniaudioengine::core
{

/** @struct MidiInputStatistics
 *  @brief Statistics related to MIDI input processing.
 */
class MidiInputStatistics : public IDataPlaneStatistics
{
public:
  size_t total_messages_processed = 0;

  MidiInputStatistics() = default;
  MidiInputStatistics(const MidiInputStatistics&) = default;
  MidiInputStatistics& operator=(const MidiInputStatistics&) = default;
  virtual ~MidiInputStatistics() = default;

  void reset() override
  {
    total_messages_processed = 0;
  }

  std::string to_string() const override
  {
    return "MidiInputStatistics(\n  Total Messages Processed = " + std::to_string(total_messages_processed) + ")";
  }
};

using MidiInputStatisticsPtr = std::shared_ptr<MidiInputStatistics>;

/** @class MidiDataPlane
 *  @brief Data plane for handling MIDI messages for a track. The Data plane is only a callback
 *  target for RtMidi and is not a producer/consumer of MIDI messages itself.
 */
class MidiDataPlane : public IDataPlane
{
public:
  MidiDataPlane(): IDataPlane()
  {
    m_name = "MidiDataPlane";
    p_statistics = std::make_shared<MidiInputStatistics>();
  }

  /** @brief Process an incoming MIDI message. Called from the RtMidi callback function.
   *  @param midi_message The MIDI message to process.
   */
  void process_midi_message(const midi::MidiMessage& midi_message);

private:
  void update_midi_input_statistics(const midi::MidiMessage& midi_message);

  bool _start() override
  {
    return true;
  }

  bool _stop() override
  {
    return true;
  }
};

typedef std::shared_ptr<MidiDataPlane> MidiDataPlanePtr;

} // namespace miniaudioengine::core

#endif // __TRACK_MIDI_DATA_PLANE_H__
