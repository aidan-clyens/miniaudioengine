#ifndef __MANAGER_H__
#define __MANAGER_H__

namespace miniaudioengine::core
{

/** @class Manager
 *  @brief Base class for all manager classes in the framework.
 *  This class provides common functionality and interface for all manager classes.
 */
class IManager
{
public:
  IManager() = default;
  virtual ~IManager() = default;

  // Common manager functionality can be added here
};

} // namespace miniaudioengine::core

#endif // __MANAGER_H__