#ifndef __CORE_ENGINE_H__
#define __CORE_ENGINE_H__

#include "engine.h"

namespace Core
{

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
  static CoreEngine &instance()
  {
    static CoreEngine instance;
    return instance;
  }

  void start_thread();
  void stop_thread();

private:
  CoreEngine(): IEngine<CoreEngineMessage>("CoreEngineThread") {}

  void run() override;
  void handle_messages() override;
};

}; // namespace Core

#endif // __CORE_ENGINE_H__