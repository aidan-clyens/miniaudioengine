#include "controller.h"
#include "dataplane.h"

#include "device.h"
#include "logger.h"

using namespace miniaudioengine::core;

void IController::set_output_device(IDevicePtr device)
{
  if (!device)
  {
    LOG_ERROR(m_name, ": Attempted to set a null output device.");
    throw std::invalid_argument(m_name + ": Output device cannot be null.");
  }

  if (!device->is_output())
  {
    LOG_ERROR(m_name, ": Device ", device->name, " is not an output device.");
    throw std::invalid_argument(m_name + ": Device " + device->name + " is not an output device.");
  }

  LOG_DEBUG(m_name, ": Output device set to ", device->to_string());
  p_device = device;
}

void IController::register_dataplane(IDataPlanePtr data_plane)
{
  LOG_DEBUG(m_name, ": Registering dataplane ", data_plane->to_string());
  p_data_planes.push_back(data_plane);
}

/** @brief Start the controller's processing.
 *  @return True if the controller started successfully, false otherwise.
 */
bool IController::start()
{
  if (m_stream_state == eStreamState::Playing)
  {
    LOG_WARNING(m_name, ": Already playing. Start command ignored.");
    return false;
  }

  try
  {
    _start();
    m_stream_state = eStreamState::Playing;
    LOG_DEBUG(m_name, ": Started successfully.");
  }
  catch (const std::exception &ex)
  {
    LOG_ERROR(m_name, ": Exception during start: ", ex.what());
    return false;
  }

  return true;
}

/** @brief Stop the controller's processing.
 *  @return True if the controller stopped successfully, false otherwise.
 */
bool IController::stop()
{
  if (m_stream_state == eStreamState::Stopped)
  {
    LOG_WARNING(m_name, ": Already stopped. Stop command ignored.");
    return false;
  }

  try
  {
    _stop();
    m_stream_state = eStreamState::Stopped;
    LOG_DEBUG(m_name, ": Stopped successfully.");
  }
  catch (const std::exception &ex)
  {
    LOG_ERROR(m_name, ": Exception during stop: ", ex.what());
    return false;
  }

  return true;
}
