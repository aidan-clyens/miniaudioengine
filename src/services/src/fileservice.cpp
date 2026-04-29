#include "fileservice.h"
#include "logger.h"

using namespace miniaudioengine;

/** @brief Lists the contents of a directory.
 *  @param path The path to the directory to list.
 *  @param type The type of contents to list (directories, files, or all).
 *  @return A vector of fileservice paths representing the contents of the directory.
 *  @throws std::runtime_error if the path does not exist or is not a directory.
 */ 
std::vector<std::filesystem::path> FileService::list_directory(const std::filesystem::path &path, PathType type) const
{
  std::vector<std::filesystem::path> contents;

  std::filesystem::path normalized_path = std::filesystem::weakly_canonical(path);
  std::filesystem::path absolute_path = convert_to_absolute(path);

  if (!path_exists(absolute_path) || !is_directory(absolute_path))
  {
    throw std::runtime_error("Path does not exist or is not a directory: " + absolute_path.string());
  }

  for (const auto& entry : std::filesystem::directory_iterator(absolute_path))
  {
    std::filesystem::path entry_path = entry.path().lexically_normal();

    switch (type)
    {
      case PathType::Directory:
        if (is_directory(entry_path))
          contents.push_back(entry_path);
        break;
      case PathType::File:
        if (is_file(entry_path))
          contents.push_back(entry_path);
        break;
      case PathType::All:
        contents.push_back(entry_path);
        break;
      default:
        throw std::invalid_argument("Invalid PathType specified.");
    }
  }

  return contents;
}

/** @brief Lists WAV files in a specified directory.
 *  @param path The path to the directory to list.
 *  @return A vector of fileservice paths representing the WAV files in the specified directory.
 *  @throws std::runtime_error if the path does not exist or is not a directory
 */
std::vector<std::filesystem::path> FileService::list_wav_files_in_directory(const std::filesystem::path &path) const
{
  std::vector<std::filesystem::path> contents = list_directory(path, PathType::File);
  std::vector<std::filesystem::path> wav_files;

  for (const auto& entry : contents)
  {
    if (is_wav_file(entry))
    {
      wav_files.push_back(entry);
    }
  }

  return wav_files;
}

/** @brief Lists MIDI files in a specified directory.
 *  @param path The path to the directory to list.
 *  @return A vector of fileservice paths representing the MIDI files in the specified directory.
 *  @throws std::runtime_error if the path does not exist or is not a directory
 */
std::vector<std::filesystem::path> FileService::list_midi_files_in_directory(const std::filesystem::path &path) const
{
  std::vector<std::filesystem::path> contents = list_directory(path, PathType::File);
  std::vector<std::filesystem::path> midi_files;

  for (const auto &entry : contents)
  {
    if (is_midi_file(entry))
    {
      midi_files.push_back(entry);
    }
  }

  return midi_files;
}

FileList FileService::get_audio_files(const std::filesystem::path &directory) const
{
  FileList audio_files;
  std::vector<std::filesystem::path> wav_paths = list_wav_files_in_directory(directory);

  for (const auto& wav_path : wav_paths)
  {
    FilePtr audio_file = get_audio_file(wav_path);
    if (audio_file)
    {
      audio_files.push_back(audio_file);
    }
  }

  return audio_files;
}

FileList FileService::get_midi_files(const std::filesystem::path &directory) const
{
  FileList midi_files;
  std::vector<std::filesystem::path> midi_paths = list_midi_files_in_directory(directory);

  for (const auto& midi_path : midi_paths)
  {
    FilePtr midi_file = get_midi_file(midi_path);
    if (midi_file)
    {
      midi_files.push_back(midi_file);
    }
  }

  return midi_files;
}

FilePtr FileService::get_audio_file(const std::filesystem::path &file_path) const
{
  FilePtr audio_file = read_wav_file(file_path);
  return audio_file;
}

FilePtr FileService::get_midi_file(const std::filesystem::path &file_path) const
{
  FilePtr midi_file = read_midi_file(file_path);
  return midi_file;
}

void FileService::save_to_wav_file(std::vector<float> audio_buffer, const std::filesystem::path &path)
{
  (void)audio_buffer; // Suppress unused variable warning
  (void)path; // Suppress unused variable warning
}

/** @brief Loads audio data from a WAV file.
 *  @param path The path to the WAV file to load.
 *  @return An AudioFile object containing the loaded audio data.
 */
FilePtr FileService::read_wav_file(const std::filesystem::path &path) const
{
  std::filesystem::path normalized_path = std::filesystem::weakly_canonical(path);
  std::filesystem::path absolute_path = convert_to_absolute(normalized_path);

  if (!path_exists(absolute_path))
  {
    LOG_ERROR("WAV file does not exist: ", absolute_path.string());
    return nullptr;
  }

  if (!is_wav_file(absolute_path))
  {
    LOG_ERROR("File is not a WAV file: ", absolute_path.string());
    return nullptr;
  }

  try
  {
    return FileHandleFactory::make_wav(absolute_path);
  }
  catch (const std::exception& ex)
  {
    LOG_ERROR("Failed to open WAV file: ", ex.what());
    return nullptr;
  }
}

FilePtr FileService::read_midi_file(const std::filesystem::path &path) const
{
  std::filesystem::path normalized_path = std::filesystem::weakly_canonical(path);
  std::filesystem::path absolute_path = convert_to_absolute(normalized_path);

  if (!path_exists(absolute_path) || !is_midi_file(absolute_path))
  {
    LOG_ERROR("MIDI file does not exist or is not a file: ", absolute_path.string());
    return nullptr;
  }

  return FileHandleFactory::make_midi(absolute_path);
}

void FileService::create_directory(const std::filesystem::path &path)
{
  std::filesystem::path normalized_path = std::filesystem::weakly_canonical(path);
  std::filesystem::path absolute_path = convert_to_absolute(normalized_path);

  LOG_INFO("Creating directory: ", absolute_path.string());
  std::error_code ec;

  if (!std::filesystem::create_directory(absolute_path, ec))
  {
    LOG_ERROR("Failed to create directory: ", absolute_path.string(), " Error: ", ec.message());
  }
}

void FileService::create_sub_directory(const std::filesystem::path &parent_path, const std::string &subdir_name)
{
  // Ensure parent directory exists
  std::filesystem::path normalized_parent_path = std::filesystem::weakly_canonical(parent_path);
  std::filesystem::path absolute_parent_path = convert_to_absolute(normalized_parent_path);

  if (!path_exists(absolute_parent_path) || !is_directory(absolute_parent_path))
  {
    LOG_ERROR("Parent directory does not exist or is not a directory: ", absolute_parent_path.string());
    return;
  }

  std::filesystem::path subdir_path = absolute_parent_path / subdir_name;;
  LOG_INFO("Creating sub-directory: ", subdir_path.string());

  create_directory(subdir_path);
}
