#ifndef __FILE_HANDLE_FACTORY_H__
#define __FILE_HANDLE_FACTORY_H__

#include "file.h"

#include <filesystem>
#include <memory>

namespace miniaudioengine
{

/** @class FileHandleFactory
 *  @brief Internal factory for constructing File objects.
 *  Not part of the public API. Only used within the library.
 */
class FileHandleFactory
{
public:
  /** @brief Open a WAV (or other libsndfile-compatible) file for reading.
   *  @param path Absolute path to the audio file.
   *  @return Shared pointer to the constructed File.
   *  @throws std::runtime_error if the file cannot be opened by libsndfile.
   */
  static FileHandlePtr make_wav(const std::filesystem::path& path);

  /** @brief Create a MIDI file handle (stub — no data is parsed yet).
   *  @param path Absolute path to the MIDI file.
   *  @return Shared pointer to the constructed File.
   */
  static FileHandlePtr make_midi(const std::filesystem::path& path);
};

} // namespace miniaudioengine

#endif // __FILE_HANDLE_FACTORY_H__
