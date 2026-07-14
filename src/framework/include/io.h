#ifndef __INPUT_OUTPUT_H__
#define __INPUT_OUTPUT_H__

#include <string>
#include <memory>

namespace miniaudioengine::framework
{

/** @enum eInputOutputType
 *  @brief Defines the "type" of input. e.g. Device or File
 */
enum eInputOutputType
{
  Device,
  File,
  None
};

enum eInputOutputDirection
{
  Input,
  Output
};

/** @class IInputOutput
 *  @brief This is an abstract interface designated the derived object is an audio or MIDI I/O interface.
 */
class IInputOutput
{
public:
  IInputOutput(const eInputOutputType type): m_io_type(type) {}
  virtual ~IInputOutput() = default;

  virtual std::string to_string() const = 0;

  eInputOutputType get_type() const
  {
    return m_io_type;
  }

  void set_direction(const eInputOutputDirection &direction)
  {
    m_direction = direction;
  }

  eInputOutputDirection get_direction() const
  {
    return m_direction;
  }

  virtual bool open_stream() = 0;
  virtual bool close_stream() = 0;
  virtual bool is_stream_open() = 0;

private:
  eInputOutputType m_io_type = None;
  eInputOutputDirection m_direction = Input;
};

using IInputOutputPtr = std::shared_ptr<IInputOutput>;

} // namespace miniaudioengine::framework

#endif // __INPUT_OUTPUT_H__