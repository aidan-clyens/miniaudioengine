#ifndef __TRACK_MIDI_DATA_PLANE_H__
#define __TRACK_MIDI_DATA_PLANE_H__

#include <memory>
#include <atomic>
#include <string>

namespace MinimalAudioEngine::Control
{
// Forward declaration
struct MidiMessage;
}

namespace MinimalAudioEngine::Data
{

/** @struct MidiInputStatistics
 *  @brief Statistics related to MIDI input processing.
 */
struct MidiInputStatistics
{
  size_t total_messages_processed = 0;

  MidiInputStatistics() = default;
  MidiInputStatistics(const MidiInputStatistics&) = default;
  MidiInputStatistics& operator=(const MidiInputStatistics&) = default;
  virtual ~MidiInputStatistics() = default;

  std::string to_string() const
  {
    return "MidiInputStatistics(\n  Total Messages Processed = " + std::to_string(total_messages_processed) + ")";
  }
};

/** @class TrackMidiDataPlane
 *  @brief Data plane for handling MIDI messages for a track. The Data plane is only a callback
 *  target for RtMidi and is not a producer/consumer of MIDI messages itself.
 */
class TrackMidiDataPlane
{
public:
  /** @brief Process an incoming MIDI message. Called from the RtMidi callback function.
   *  @param midi_message The MIDI message to process.
   */  
  void process_midi_message(const Control::MidiMessage& midi_message);

  /** @brief Check if the track is currently running.
   *  @return True if running, false if stopped.
   */
  bool is_running() const
  {
    return !m_stop_command.load(std::memory_order_acquire);
  }

  /** @brief Start MIDI processing
   */
  void start()
  {
    m_stop_command.store(false, std::memory_order_release);
  }

  /** @brief Stop MIDI processing
   */
  void stop()
  {
    m_stop_command.store(true, std::memory_order_release);
  }

  /** @brief Get MIDI input statistics.
   *  @return MidiInputStatistics structure containing input statistics.
   */
  MidiInputStatistics get_statistics() const
  {
    return m_midi_input_stats;
  }

private:
  MidiInputStatistics m_midi_input_stats;

  std::atomic<bool> m_stop_command{true};

private:
  void update_midi_input_statistics(const Control::MidiMessage& midi_message); 
};

typedef std::shared_ptr<TrackMidiDataPlane> TrackMidiDataPlanePtr;

} // namespace MinimalAudioEngine::Data

#endif // __TRACK_MIDI_DATA_PLANE_H__