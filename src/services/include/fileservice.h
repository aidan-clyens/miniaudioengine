#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "file.h"

#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace miniaudioengine
{

using FilePtr = std::shared_ptr<File>;
using FileList = std::vector<FilePtr>;

/** @enum PathType
 *  @brief Enum to specify the type of path to filter when listing directory contents.
 */
enum class PathType
{
  Directory,
  File,
  All,
};

/** @class FileService
 *  @brief Singleton class for managing file system operations.
 */
class FileService
{
public:
  FileService() = default;
  ~FileService() = default;

  static FileService& instance()
  {
    static FileService instance;
    return instance;
  }

	std::vector<std::filesystem::path> list_directory(const std::filesystem::path &path, PathType type = PathType::All) const;
  std::vector<std::filesystem::path> list_wav_files_in_directory(const std::filesystem::path &path) const;
  std::vector<std::filesystem::path> list_midi_files_in_directory(const std::filesystem::path &path) const;

  FileList get_audio_files(const std::filesystem::path &directory) const;
  FileList get_midi_files(const std::filesystem::path &directory) const;

  FilePtr get_audio_file(const std::filesystem::path &file_path) const;
  FilePtr get_midi_file(const std::filesystem::path &file_path) const;

  /** @brief Checks if a specified path exists.
   *  @param path The path to check.
   *  @return True if the path exists, false otherwise.
   */
  inline bool path_exists(const std::filesystem::path &path) const
  {
    std::error_code ec;
    return std::filesystem::exists(path, ec);
  }

  /** @brief Checks if a specified path is a file.
   *  @param path The path to check.
   *  @return True if the path is a file, false otherwise.
   */
  inline bool is_file(const std::filesystem::path& path) const
  {
    std::error_code ec;
    return std::filesystem::is_regular_file(path, ec);
  }

  /** @brief Checks if a specified path is a WAV file.
   *  @param path The path to check.
   *  @return True if the path is a WAV file, false otherwise. 
   */
  inline bool is_wav_file(const std::filesystem::path& path) const
  {
    std::error_code ec;
    return (std::filesystem::is_regular_file(path, ec) && path.extension() == ".wav");
  }

  /** @brief Checks if a specified path is a MIDI file.
   *  @param path The path to check.
   *  @return True if the path is a MIDI file, false otherwise.
   */
  inline bool is_midi_file(const std::filesystem::path &path) const
  {
    std::error_code ec;
    return (std::filesystem::is_regular_file(path, ec) && path.extension() == ".mid");
  }

  /** @brief Checks if a specified path is a directory.
   *  @param path The path to check.
   *  @return True if the path is a directory, false otherwise.
   */
  inline bool is_directory(const std::filesystem::path& path) const
  {
    std::error_code ec;
    return std::filesystem::is_directory(path, ec);
  }

  /** @brief Creates a directory at the specified path.
   *  @param path The path where the directory should be created.
   */
  void create_directory(const std::filesystem::path &path);
  
  /** @brief Creates a sub-directory within a specified parent directory.
   *  @param parent_path The path to the parent directory.
   *  @param subdir_name The name of the sub-directory to create.
   */
  void create_sub_directory(const std::filesystem::path &parent_path, const std::string &subdir_name);

  /** @brief Converts a relative path to an absolute path.
   *  @param path The path to convert.
   *  @return The absolute path.
   */
  std::filesystem::path convert_to_absolute(const std::filesystem::path &path) const
  {
    return path.is_relative() ? std::filesystem::current_path() / path.lexically_normal() : path;
  }

	void save_to_wav_file(std::vector<float> audio_buffer, const std::filesystem::path &path);

  /** @brief Loads audio data from a WAV (or libsndfile-compatible audio) file.
   *  @param path The path to the audio file.
   *  @return FilePtr on success, or std::nullopt if the file could not be opened.
   */
  FilePtr read_wav_file(const std::filesystem::path &path) const;

  /** @brief Creates a File for a MIDI file.
   *  @param path The path to the MIDI file.
   *  @return FilePtr on success, or std::nullopt if the path is invalid.
   */
  FilePtr read_midi_file(const std::filesystem::path &path) const;
};

}  // namespace miniaudioengine

#endif  // __FILE_SYSTEM_H__