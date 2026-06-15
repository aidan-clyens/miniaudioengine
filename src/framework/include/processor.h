#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <string>
#include <memory>

namespace miniaudioengine::framework
{

/** @class IProcessor
 *  @brief This is an abstract interface designated the derived object is an audio or MIDI I/O interface.
 */
class IProcessor
{
public:
  /** @enum eDataType
   *  @brief Defines the "type" of input. e.g. Device or File
   */
  enum class eDataType
  {
    Audio,
    Midi,
    None
  };

  IProcessor(const eDataType type):
    m_type(type) {}

  virtual ~IProcessor() = default;

  virtual std::string to_string() const = 0;

  eDataType get_type() const
  {
    return m_type;
  }

protected:
  std::string data_type_to_string(const eDataType type) const
  {
    switch (type)
    {
      case eDataType::Audio:
        return "Audio";
      case eDataType::Midi:
        return "Midi";
      case eDataType::None:
        return "None";
      default:
        return "";
    }
  }

private:
  eDataType m_type = eDataType::None;
};

using IProcessorPtr = std::shared_ptr<IProcessor>;

} // namespace miniaudioengine::framework

#endif // __PROCESSOR_H__