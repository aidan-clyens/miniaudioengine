#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

namespace MinimalAudioEngine::Core
{

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

  // Add common controller functionality here
};


} // namespace MinimalAudioEngine::Core

#endif // __CONTROLLER_H__