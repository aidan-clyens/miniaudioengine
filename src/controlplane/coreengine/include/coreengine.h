#ifndef __CORE_ENGINE_H__
#define __CORE_ENGINE_H__

#include "engine.h"

namespace MinimalAudioEngine
{

constexpr const char *CORE_ENGINE_THREAD_NAME = "CoreEngineThread";

/** @struct CoreEngineMessage
 *  @brief Message structure for CoreEngine
 */
struct CoreEngineMessage
{
  enum class eType
  {
    Shutdown,
    Restart,
    Custom
  } type;

  std::string info; // Optional additional information
};

/** @class CoreEngine
 *  @brief CoreEngine class derived from IEngine
 */
class CoreEngine : public IEngine<CoreEngineMessage>
{
public:
  CoreEngine() : IEngine<CoreEngineMessage>(CORE_ENGINE_THREAD_NAME) {}
  ~CoreEngine() override = default;

  void start_thread();
  void stop_thread();

private:

  void run() override;
  void handle_messages() override;
};

}; // namespace MinimalAudioEngine

#endif // __CORE_ENGINE_H__