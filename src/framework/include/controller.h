#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <memory>
#include <vector>

namespace miniaudioengine::core
{

// Forward declaration
class IDataPlane;
using IDataPlanePtr = std::shared_ptr<IDataPlane>;
class IDevice;
using IDevicePtr = std::shared_ptr<IDevice>;

/** @enum eStreamState
 *  @brief IController states
 */
enum class eStreamState
{
  Idle,
  Stopped,
  Playing,
  Monitor,
};

/** @class IController
 *  @brief Base class for all controllers in the framework.
 *  Controllers are responsible for managing specific aspects of the application,
 *  such as audio, MIDI, or track management. This class provides a common interface
 *  and shared functionality for all controller types.
 */
class IController
{
public:
  virtual ~IController() = default;

  void register_dataplane(IDataPlanePtr data_plane)
  {
    p_data_planes.push_back(data_plane);
  }

  std::vector<IDataPlanePtr> get_registered_dataplanes() const
  {
    return p_data_planes;
  }

  void clear_registered_dataplane()
  {
    p_data_planes.clear();
  }

  void set_output_device(IDevicePtr device);

  IDevicePtr get_output_device() const
  {
    return p_device;
  }

  eStreamState get_stream_state() const
  {
    return m_stream_state;
  }

protected:
  eStreamState m_stream_state{eStreamState::Idle};

private:
  std::vector<IDataPlanePtr> p_data_planes;
  IDevicePtr p_device{nullptr};
};

} // namespace miniaudioengine::core

#endif // __CONTROLLER_H__