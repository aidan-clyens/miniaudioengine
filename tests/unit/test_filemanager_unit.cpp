#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "filemanager.h"
#include "wavfile.h"
#include "midifile.h"
#include "logger.h"

using namespace MinimalAudioEngine;


TEST(FileSystemTest, PathExists)
{
  FileManager& fs = FileManager::instance();

  std::filesystem::path existing_path = "src";
  ASSERT_TRUE(fs.path_exists(existing_path)) << "Existing path should return true.";

  std::filesystem::path non_existing_path = "/non/existing/path";
  ASSERT_FALSE(fs.path_exists(non_existing_path)) << "Non-existing path should return false.";
}

TEST(FileSystemTest, IsFile)
{
  FileManager& fs = FileManager::instance();

  std::filesystem::path file_path = "./README.md";
  ASSERT_TRUE(fs.is_file(file_path)) << "Path should be recognized as a file.";

  std::filesystem::path directory_path = "src";
  ASSERT_FALSE(fs.is_file(directory_path)) << "Path should not be recognized as a file.";
}

TEST(FileSystemTest, IsWavFile)
{
  FileManager& fs = FileManager::instance();

  std::filesystem::path wav_file_path = "./samples/test.wav";
  ASSERT_TRUE(fs.is_wav_file(wav_file_path)) << "Path should be recognized as a WAV file.";

  std::filesystem::path non_wav_file_path = "./README.md";
  ASSERT_FALSE(fs.is_wav_file(non_wav_file_path)) << "Path should not be recognized as a WAV file.";
}

TEST(FileSystemTest, IsMidiFile)
{
  FileManager &fs = FileManager::instance();

  std::filesystem::path midi_file_path = "./samples/midi_c_major_monophonic.mid";
  ASSERT_TRUE(fs.is_midi_file(midi_file_path)) << "Path should be recognized as a MIDI file.";

  std::filesystem::path non_midi_file_path = "./README.md";
  ASSERT_FALSE(fs.is_midi_file(non_midi_file_path)) << "Path should not be recognized as a MIDI file.";
}

TEST(FileSystemTest, IsDirectory)
{
  FileManager& fs = FileManager::instance();

  std::filesystem::path directory_path = "src";
  ASSERT_TRUE(fs.is_directory(directory_path)) << "Path should be recognized as a directory.";

  std::filesystem::path file_path = "./README.md";
  ASSERT_FALSE(fs.is_directory(file_path)) << "Path should not be recognized as a directory.";
}

TEST(FileSystemTest, ListDirectory)
{
  FileManager& fs = FileManager::instance();
  std::filesystem::path path = "./";

  // Get all contents from directory
  std::vector<std::filesystem::path> contents = fs.list_directory(path);
  ASSERT_FALSE(contents.empty()) << "Directory should not be empty.";

  unsigned int contents_count = contents.size();

  LOG_INFO("Contents of directory: ", path.string());
  for (const auto& entry : contents)
  {
    LOG_INFO(entry.string());
  }

  // Filter by directories
  contents = fs.list_directory(path, PathType::Directory);
  ASSERT_FALSE(contents.empty()) << "Directory should contain directories.";
  ASSERT_NE(contents.size(), contents_count) << "Contents count should not be equal after filtering by directories.";

  // Filter by files
  contents = fs.list_directory(path, PathType::File);
  ASSERT_FALSE(contents.empty()) << "Directory should contain files.";
  ASSERT_NE(contents.size(), contents_count) << "Contents count should not be equal after filtering by files.";
}

TEST(FileSystemTest, ListWavFilesInDirectory)
{
  FileManager& fs = FileManager::instance();

  std::filesystem::path path = "./samples";

  std::vector<std::filesystem::path> wav_files = fs.list_wav_files_in_directory(path);
  ASSERT_FALSE(wav_files.empty()) << "WAV files should not be empty.";

  LOG_INFO("WAV files in directory: ", path.string());
  for (const auto& file : wav_files)
  {
    ASSERT_TRUE(fs.is_file(file)) << file.string() << " should be a file.";
    ASSERT_EQ(file.extension(), ".wav") << file.string() << " should have .wav extension.";
    LOG_INFO(file.string());
  }
}

TEST(FileSystemTest, ListMidiFilesInDirectory)
{
  FileManager &fs = FileManager::instance();

  std::filesystem::path path = "./samples";

  std::vector<std::filesystem::path> midi_files = fs.list_midi_files_in_directory(path);
  ASSERT_FALSE(midi_files.empty()) << "MIDI files should not be empty.";

  LOG_INFO("MIDI files in directory: ", path.string());
  for (const auto &file : midi_files)
  {
    ASSERT_TRUE(fs.is_file(file)) << file.string() << " should be a file.";
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
  FileManager& fs = FileManager::instance();

  // Get a WAV file path
  std::vector<std::filesystem::path> samples = fs.list_wav_files_in_directory("./samples");
  for (const auto& sample : samples)
  {
    LOG_INFO(sample.string());
  }

  std::filesystem::path wav_file_path = "./samples/test.wav";
  ASSERT_TRUE(fs.path_exists(wav_file_path)) << "WAV file should exist.";

  // Load the WAV file
  std::shared_ptr<WavFile> file = fs.read_wav_file(wav_file_path);

  ASSERT_EQ(file->get_filepath(), fs.convert_to_absolute(wav_file_path)) << "Loaded WAV file path should match the original path.";

  ASSERT_TRUE(fs.path_exists(file->get_filepath())) << "Loaded file should exist.";
  ASSERT_TRUE(fs.is_wav_file(file->get_filepath())) << "Loaded file should be a WAV file.";
}

TEST(FileSystemTest, LoadMidiFile)
{
  ASSERT_EQ(1, 0) << "This is a placeholder test for loading a MIDI file.";
}