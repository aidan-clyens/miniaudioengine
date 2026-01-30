#include "controller.h"
#include "dataplane.h"

#include "device.h"
#include "logger.h"

using namespace miniaudioengine::core;

void IController::set_output_device(IDevicePtr device)
{
  if (!device)
  {
    LOG_ERROR("IController: Attempted to set a null output device.");
    throw std::invalid_argument("IController: Output device cannot be null.");
  }

  if (!device->is_output())
  {
    LOG_ERROR("IController: Device ", device->name, " is not an output device.");
    throw std::invalid_argument("IController: Device " + device->name + " is not an output device.");
  }

  LOG_DEBUG("IController: Output device set to ", device->to_string());
  p_device = device;
}

void IController::register_dataplane(IDataPlanePtr data_plane)
{
  LOG_DEBUG("IController: Registering dataplane ", data_plane->to_string());
  p_data_planes.push_back(data_plane);
}