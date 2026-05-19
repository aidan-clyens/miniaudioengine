#ifndef __INPUT_OUTPUT_H__
#define __INPUT_OUTPUT_H__

#include <string>
#include <memory>

namespace miniaudioengine::framework
{

/** @enum eInputOutputType
 *  @brief Defines the "type" of output. e.g. Device or File
 */
enum eInputOutputType
{
  eInputOutputType_Device,
  eInputOutputType_File,
  eInputOutputType_None
};

/** @class IInputOutput
 *  @brief This is an abstract interface designated the derived object is an audio/MIDI output.
 */
class IInputOutput
{
public:
  IInputOutput(const eInputOutputType type) : m_io_type(type) {}

  virtual std::string to_string() const = 0;

  eInputOutputType get_type() const
  {
    return m_io_type;
  }

private:
  eInputOutputType m_io_type = eInputOutputType_None;
};

using IOutputPtr = std::shared_ptr<IInputOutput>;

} // namespace miniaudioengine::framework

#endif // __INPUT_OUTPUT_H__