#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "manager.h"
#include "input.h"

#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace miniaudioengine::file
{

// Forward declaration
class WavFile;
class MidiFile;

// Type definitions
typedef std::shared_ptr<WavFile> WavFilePtr;
typedef std::shared_ptr<MidiFile> MidiFilePtr;

/** @class File
 *  @brief Base class for various file types 
 */
class File : public core::IInput
{
  friend class FileManager;

public:
  virtual ~File() = default;

  std::filesystem::path get_filepath() const
  {
    return m_filepath;
  }

  std::string get_filename() const
  {
    return m_filepath.filename().string();
  }

  virtual std::string to_string() const
  {
    return "File(Path=" + m_filepath.string() + ")";
  }

protected:
  File(const std::filesystem::path &path, const core::eInputType type):
    core::IInput(type),
    m_filepath(path) {}

  std::filesystem::path m_filepath;
};

/** @enum PathType
 *  @brief Enum to specify the type of path to filter when listing directory contents.
 */
enum class PathType
{
  Directory,
  File,
  All,
};

/** @class FileManager
 *  @brief Singleton class for managing file system operations.
 */
class FileManager : public core::IManager
{
public:
  static FileManager& instance()
  {
    static FileManager instance;
    return instance;
  }

	std::vector<std::filesystem::path> list_directory(const std::filesystem::path &path, PathType type = PathType::All);
  std::vector<std::filesystem::path> list_wav_files_in_directory(const std::filesystem::path &path);
  std::vector<std::filesystem::path> list_midi_files_in_directory(const std::filesystem::path &path);

  /** @brief Checks if a specified path exists.
   *  @param path The path to check.
   *  @return True if the path exists, false otherwise.
   */
  inline bool path_exists(const std::filesystem::path &path) const
  {
    return std::filesystem::exists(path);
  }

  /** @brief Checks if a specified path is a file.
   *  @param path The path to check.
   *  @return True if the path is a file, false otherwise.
   */
  inline bool is_file(const std::filesystem::path& path) const
  {
    return (path_exists(path) && std::filesystem::is_regular_file(path));
  }

  /** @brief Checks if a specified path is a WAV file.
   *  @param path The path to check.
   *  @return True if the path is a WAV file, false otherwise. 
   */
  inline bool is_wav_file(const std::filesystem::path& path) const
  {
    return (path_exists(path) && std::filesystem::is_regular_file(path) && path.extension() == ".wav");
  }

  /** @brief Checks if a specified path is a MIDI file.
   *  @param path The path to check.
   *  @return True if the path is a MIDI file, false otherwise.
   */
  inline bool is_midi_file(const std::filesystem::path &path) const
  {
    return (path_exists(path) && std::filesystem::is_regular_file(path) && path.extension() == ".mid");
  }

  /** @brief Checks if a specified path is a directory.
   *  @param path The path to check.
   *  @return True if the path is a directory, false otherwise.
   */
  inline bool is_directory(const std::filesystem::path& path) const
  {
    return (path_exists(path) && std::filesystem::is_directory(path));
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
  std::optional<WavFilePtr> read_wav_file(const std::filesystem::path &path);
  std::optional<MidiFilePtr> read_midi_file(const std::filesystem::path &path);

private:
  FileManager() = default;
  virtual ~FileManager() = default;

  FileManager(const FileManager&) = delete;
  FileManager& operator=(const FileManager&) = delete;
};

}  // namespace miniaudioengine::file

#endif  // __FILE_SYSTEM_H__