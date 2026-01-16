#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "filemanager.h"
#include "wavfile.h"
#include "midifile.h"
#include "logger.h"

using namespace MinimalAudioEngine;
using namespace MinimalAudioEngine::Control;

static const std::filesystem::path PROJECT_ROOT_PATH("../../../../");

TEST(FileSystemTest, ListDirectory)
{
  std::filesystem::path path = PROJECT_ROOT_PATH;

  // Get all contents from directory
  std::vector<std::filesystem::path> contents = FileManager::instance().list_directory(path);
  ASSERT_FALSE(contents.empty()) << "Directory '" + path.string() + "' should not be empty.";

  unsigned int contents_count = contents.size();

  LOG_INFO("Contents of directory: ", path.string());
  for (const auto &entry : contents)
  {
    LOG_INFO(entry.string());
  }

  // Filter by directories
  contents = FileManager::instance().list_directory(path, PathType::Directory);
  ASSERT_FALSE(contents.empty()) << "Directory '" + path.string() + "' should contain directories.";
  ASSERT_NE(contents.size(), contents_count) << "Contents count should not be equal after filtering by directories.";

  // Filter by files
  contents = FileManager::instance().list_directory(path, PathType::File);
  ASSERT_FALSE(contents.empty()) << "Directory '" + path.string() + "' should contain files.";
  ASSERT_NE(contents.size(), contents_count) << "Contents count should not be equal after filtering by files.";
}

TEST(FileSystemTest, PathExists)
{
  std::filesystem::path existing_path = PROJECT_ROOT_PATH / "examples";
  ASSERT_TRUE(FileManager::instance().path_exists(existing_path)) << "Existing path '" + existing_path.string() + "' should return true.";

  std::filesystem::path non_existing_path = "/non/existing/path";
  ASSERT_FALSE(FileManager::instance().path_exists(non_existing_path)) << "Non-existing path '" + non_existing_path.string() + "' should return false.";
}

TEST(FileSystemTest, IsFile)
{
  std::filesystem::path file_path = PROJECT_ROOT_PATH / "README.md";
  ASSERT_TRUE(FileManager::instance().is_file(file_path)) << "Path '" + file_path.string() + "' should be recognized as a file.";

  std::filesystem::path directory_path = PROJECT_ROOT_PATH / "src";
  ASSERT_FALSE(FileManager::instance().is_file(directory_path)) << "Path '" + directory_path.string() + "' should not be recognized as a file.";
}

TEST(FileSystemTest, IsWavFile)
{
  std::filesystem::path wav_file_path = PROJECT_ROOT_PATH / "samples/test.wav";
  ASSERT_TRUE(FileManager::instance().is_wav_file(wav_file_path)) << "Path '" + wav_file_path.string() + "' should be recognized as a WAV file.";

  std::filesystem::path non_wav_file_path = PROJECT_ROOT_PATH / "README.md";
  ASSERT_FALSE(FileManager::instance().is_wav_file(non_wav_file_path)) << "Path '" + non_wav_file_path.string() + "' should not be recognized as a WAV file.";
}

TEST(FileSystemTest, IsMidiFile)
{
  std::filesystem::path midi_file_path = PROJECT_ROOT_PATH / "samples/midi_c_major_monophonic.mid";
  ASSERT_TRUE(FileManager::instance().is_midi_file(midi_file_path)) << "Path '" + midi_file_path.string() + "' should be recognized as a MIDI file.";

  std::filesystem::path non_midi_file_path = PROJECT_ROOT_PATH / "README.md";
  ASSERT_FALSE(FileManager::instance().is_midi_file(non_midi_file_path)) << "Path '" + non_midi_file_path.string() + "' should not be recognized as a MIDI file.";
}

TEST(FileSystemTest, IsDirectory)
{
  std::filesystem::path directory_path = PROJECT_ROOT_PATH / "src";
  ASSERT_TRUE(FileManager::instance().is_directory(directory_path)) << "Path '" + directory_path.string() + "' should be recognized as a directory.";

  std::filesystem::path file_path = PROJECT_ROOT_PATH / "README.md";
  ASSERT_FALSE(FileManager::instance().is_directory(file_path)) << "Path '" + file_path.string() + "' should not be recognized as a directory.";
}

TEST(FileSystemTest, ListWavFilesInDirectory)
{
  std::filesystem::path path = PROJECT_ROOT_PATH / "samples";

  std::vector<std::filesystem::path> wav_files = FileManager::instance().list_wav_files_in_directory(path);
  ASSERT_FALSE(wav_files.empty()) << "WAV files in directory '" + path.string() + "' should not be empty.";

  LOG_INFO("WAV files in directory: ", path.string());
  for (const auto& file : wav_files)
  {
    ASSERT_TRUE(FileManager::instance().is_file(file)) << file.string() << " should be a file.";
    ASSERT_EQ(file.extension(), ".wav") << file.string() << " should have .wav extension.";
    LOG_INFO(file.string());
  }
}

TEST(FileSystemTest, ListMidiFilesInDirectory)
{
  std::filesystem::path path = PROJECT_ROOT_PATH / "samples";

  std::vector<std::filesystem::path> midi_files = FileManager::instance().list_midi_files_in_directory(path);
  ASSERT_FALSE(midi_files.empty()) << "MIDI files in directory '" + path.string() + "' should not be empty.";

  LOG_INFO("MIDI files in directory: ", path.string());
  for (const auto &file : midi_files)
  {
    ASSERT_TRUE(FileManager::instance().is_file(file)) << file.string() << " should be a file.";
    ASSERT_EQ(file.extension(), ".mid") << file.string() << " should have .mid extension.";
    LOG_INFO(file.string());
  }
}

TEST(FileSystemTest, SaveToWavFile)
{
  ASSERT_EQ(1, 0) << "This is a placeholder test for saving to a WAV file.";
}

TEST(FileSystemTest, LoadWavFile)
{
  // Get a WAV file path
  std::vector<std::filesystem::path> samples = FileManager::instance().list_wav_files_in_directory(PROJECT_ROOT_PATH / "samples");
  for (const auto& sample : samples)
  {
    LOG_INFO(sample.string());
  }

  std::filesystem::path wav_file_path = PROJECT_ROOT_PATH / "samples/test.wav";
  ASSERT_TRUE(FileManager::instance().path_exists(wav_file_path)) << "WAV file '" + wav_file_path.string() + "' should exist.";

  // Load the WAV file
  auto file = FileManager::instance().read_wav_file(wav_file_path);
  ASSERT_TRUE(file.has_value()) << "WAV file '" + wav_file_path.string() + "' should be loaded successfully.";
  WavFilePtr file_ptr = file.value();

  ASSERT_EQ(file_ptr->get_filepath(), FileManager::instance().convert_to_absolute(wav_file_path)) << "Loaded WAV file path should match the original path.";

  ASSERT_TRUE(FileManager::instance().path_exists(file_ptr->get_filepath())) << "Loaded file '" + file_ptr->get_filepath().string() + "' should exist.";
  ASSERT_TRUE(FileManager::instance().is_wav_file(file_ptr->get_filepath())) << "Loaded file '" + file_ptr->get_filepath().string() + "' should be a WAV file.";
}

TEST(FileSystemTest, LoadMidiFile)
{
  ASSERT_EQ(1, 0) << "This is a placeholder test for loading a MIDI file.";
}

TEST(FileSystemTest, CreateDirectory)
{
  const std::string dir_name = "test_directory";

  std::filesystem::path created_dir_path = PROJECT_ROOT_PATH / dir_name;
  FileManager::instance().create_directory(created_dir_path);

  ASSERT_TRUE(FileManager::instance().path_exists(created_dir_path)) << "Created directory '" + created_dir_path.string() + "' should exist.";
  ASSERT_TRUE(FileManager::instance().is_directory(created_dir_path)) << "Created path '" + created_dir_path.string() + "' should be a directory.";
}

TEST(FileSystemTest, CreateSubDirectory)
{
  const std::string parent_dir_name = "test_parent_directory";
  const std::string sub_dir_name = "test_sub_directory";

  std::filesystem::path parent_dir_path = PROJECT_ROOT_PATH / parent_dir_name;
  FileManager::instance().create_directory(parent_dir_path);
  ASSERT_TRUE(FileManager::instance().path_exists(parent_dir_path)) << "Parent directory '" + parent_dir_path.string() + "' should exist.";

  FileManager::instance().create_sub_directory(parent_dir_path, sub_dir_name);
  std::filesystem::path sub_dir_path = parent_dir_path / sub_dir_name;

  ASSERT_TRUE(FileManager::instance().path_exists(sub_dir_path)) << "Sub-directory '" + sub_dir_path.string() + "' should exist.";
  ASSERT_TRUE(FileManager::instance().is_directory(sub_dir_path)) << "Sub-directory path '" + sub_dir_path.string() + "' should be a directory.";
}