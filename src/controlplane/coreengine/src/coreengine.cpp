#include "coreengine.h"

#include "audiostreamcontroller.h"
#include "midiengine.h"

using namespace MinimalAudioEngine;

void CoreEngine::start_thread()
{
  IEngine<CoreEngineMessage>::start_thread();
  MinimalAudioEngine::MidiEngine::instance().start_thread();
}

void CoreEngine::stop_thread()
{
  IEngine<CoreEngineMessage>::stop_thread();
  MinimalAudioEngine::MidiEngine::instance().stop_thread();
}

void CoreEngine::run()
{
  while (is_running())
  {
    handle_messages();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void CoreEngine::handle_messages()
{
  CoreEngineMessage message;
  while (auto opt_message = pop_message())
  {
    message = *opt_message;
    switch (message.type)
    {
      case CoreEngineMessage::eType::Shutdown:
        LOG_INFO("CoreEngine: Received Shutdown message");
        // Handle shutdown logic here
        break;
      case CoreEngineMessage::eType::Restart:
        LOG_INFO("CoreEngine: Received Restart message");
        // Handle restart logic here
        break;
      case CoreEngineMessage::eType::Custom:
        LOG_INFO("CoreEngine: Received Custom message - ", message.info);
        // Handle custom message logic here
        break;
      default:
        LOG_ERROR("CoreEngine: Unknown message type received");
        break;
    }
  }
}