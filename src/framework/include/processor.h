#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

namespace miniaudioengine::core
{

/** @class IProcessor
 *  @brief Base class for all processors in the framework.
 *  Processors are responsible for handling data processing,
 *  such as audio processing, MIDI processing, or effects processing.
 *  concurrently to main application.
 *  This class provides a common interface and shared functionality for all processor types.
 */
class IProcessor
{
public:
  virtual ~IProcessor() = default;

  // Add common processor functionality here
};

};

#endif // __PROCESSOR_H__