#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <memory>
#include <vector>

#include "logger.h"

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
  IController(const std::string &name) : m_name(name) {}
  IController(const IController &) = delete; // Disable copy constructor
  virtual ~IController() = default;

  /** @brief Register a data plane with the controller.
   *  @param data_plane Shared pointer to an IDataPlane object to register.
   */
  void register_dataplane(IDataPlanePtr data_plane);

  /** @brief Get a list of all registered data planes.
   *  @return A vector of shared pointers to the registered IDataPlane objects.
   */
  std::vector<IDataPlanePtr> get_registered_dataplanes() const
  {
    return p_data_planes;
  }

  /** @brief Clear all registered data planes from the controller.
   */
  void clear_registered_dataplane()
  {
    p_data_planes.clear();
  }

  /** @brief Set the output device object
   *  @param device Shared pointer to an IDevice object representing the output device to use.
   *  @throws std::invalid_argument if the provided device is null or not an output device.
   */
  void set_output_device(IDevicePtr device);

  IDevicePtr get_output_device() const
  {
    return p_device;
  }

  /** @brief Get the current stream state of the controller.
   *  @return The current stream state as an eStreamState enum value.
   */
  eStreamState get_stream_state() const
  {
    return m_stream_state;
  }

  /** @brief Start the controller's processing.
   *  @return True if the controller started successfully, false otherwise.
   */
  bool start();

  /** @brief Stop the controller's processing.
   *  @return True if the controller stopped successfully, false otherwise.
   */
  bool stop();

protected:
  eStreamState m_stream_state{eStreamState::Stopped};

  /** @brief Start the controller's processing.
   *  This is a pure virtual function that must be implemented by derived classes to define
   *  the specific behavior for starting the controller.
   *  @return True if the controller started successfully, false otherwise.
   *  @throws std::exception if an error occurs during startup.
   */
  virtual bool _start() = 0;

  /** @brief Stop the controller's processing.
   *  This is a pure virtual function that must be implemented by derived classes to define
   *  the specific behavior for stopping the controller.
   *  @return True if the controller stopped successfully, false otherwise.
   *  @throws std::exception if an error occurs during shutdown.
   */
  virtual bool _stop() = 0;

private:
  std::vector<IDataPlanePtr> p_data_planes;
  IDevicePtr p_device{nullptr};

  std::string m_name{"IController"};
};

} // namespace miniaudioengine::core

#endif // __CONTROLLER_H__