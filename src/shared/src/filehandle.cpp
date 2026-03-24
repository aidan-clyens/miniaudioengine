#include "filehandle_factory.h"
#include "miniaudioengine/filehandle.h"

#include <sndfile.h>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace miniaudioengine
{

// =============================================================================
// FileHandle::Impl — defined here so sndfile headers stay out of the public API
// =============================================================================

struct FileHandle::Impl
{
  FileHandle::eFileType file_type;
  std::filesystem::path filepath;

  // WAV-only fields (zeroed/empty for MIDI)
  SF_INFO sfinfo{};
  std::shared_ptr<SNDFILE> sndfile;
};

// =============================================================================
// FileHandle — member implementations
// =============================================================================

FileHandle::FileHandle(std::unique_ptr<Impl> impl)
  : p_impl(std::move(impl)) {}

FileHandle::~FileHandle() = default;

FileHandle::eFileType FileHandle::get_file_type() const { return p_impl->file_type; }

std::filesystem::path FileHandle::get_filepath() const { return p_impl->filepath; }

std::string FileHandle::get_filename() const { return p_impl->filepath.filename().string(); }

unsigned int FileHandle::get_total_frames() const
{
  if (p_impl->file_type != eFileType::Wav) return 0u;
  return static_cast<unsigned int>(p_impl->sfinfo.frames);
}

unsigned int FileHandle::get_bits_per_sample() const
{
  if (p_impl->file_type != eFileType::Wav) return 0u;
  const int fmt = p_impl->sfinfo.format & SF_FORMAT_SUBMASK;
  switch (fmt)
  {
    case SF_FORMAT_PCM_16: return 16u;
    case SF_FORMAT_PCM_24: return 24u;
    case SF_FORMAT_PCM_32: return 32u;
    case SF_FORMAT_FLOAT:  return 32u;
    case SF_FORMAT_DOUBLE: return 64u;
    default:               return 0u;
  }
}

unsigned int FileHandle::get_sample_rate() const
{
  if (p_impl->file_type != eFileType::Wav) return 0u;
  return static_cast<unsigned int>(p_impl->sfinfo.samplerate);
}

unsigned int FileHandle::get_channels() const
{
  if (p_impl->file_type != eFileType::Wav) return 0u;
  return static_cast<unsigned int>(p_impl->sfinfo.channels);
}

double FileHandle::get_duration_seconds() const
{
  if (p_impl->file_type != eFileType::Wav) return 0.0;
  const unsigned int sr = get_sample_rate();
  return sr > 0 ? static_cast<double>(get_total_frames()) / static_cast<double>(sr) : 0.0;
}

std::string FileHandle::get_format_string() const
{
  if (p_impl->file_type != eFileType::Wav) return {};
  switch (p_impl->sfinfo.format & SF_FORMAT_TYPEMASK)
  {
    case SF_FORMAT_WAV:  return "WAV";
    case SF_FORMAT_AIFF: return "AIFF";
    case SF_FORMAT_FLAC: return "FLAC";
    default:             return "Unknown";
  }
}

long long FileHandle::read_frames(std::vector<float>& buffer, long long frames_to_read)
{
  if (p_impl->file_type != eFileType::Wav || !p_impl->sndfile) return 0LL;
  return sf_readf_float(p_impl->sndfile.get(),
                        buffer.data(),
                        static_cast<sf_count_t>(frames_to_read));
}

void FileHandle::seek(long long frame_offset)
{
  if (p_impl->file_type != eFileType::Wav || !p_impl->sndfile) return;
  sf_seek(p_impl->sndfile.get(), static_cast<sf_count_t>(frame_offset), SEEK_SET);
}

std::string FileHandle::to_string() const
{
  if (p_impl->file_type == eFileType::Wav)
  {
    return "FileHandle(Type=Wav"
           ", Path=" + p_impl->filepath.string() +
           ", TotalFrames=" + std::to_string(get_total_frames()) +
           ", DurationSeconds=" + std::to_string(get_duration_seconds()) +
           ", Format=" + get_format_string() +
           ", SampleRate=" + std::to_string(get_sample_rate()) +
           ", BitsPerSample=" + std::to_string(get_bits_per_sample()) +
           ", Channels=" + std::to_string(get_channels()) + ")";
  }
  return "FileHandle(Type=Midi, Path=" + p_impl->filepath.string() + ")";
}

// =============================================================================
// FileHandleFactory
// =============================================================================

FileHandlePtr FileHandleFactory::make_wav(const std::filesystem::path& path)
{
  auto impl = std::make_unique<FileHandle::Impl>();
  impl->file_type = FileHandle::eFileType::Wav;
  impl->filepath  = path;
  impl->sfinfo    = {};

  impl->sndfile = std::shared_ptr<SNDFILE>(
    sf_open(path.string().c_str(), SFM_READ, &impl->sfinfo),
    [](SNDFILE* f) { if (f) sf_close(f); });

  if (!impl->sndfile)
  {
    throw std::runtime_error("Failed to open WAV file: " + path.string());
  }

  return FileHandlePtr(new FileHandle(std::move(impl)));
}

FileHandlePtr FileHandleFactory::make_midi(const std::filesystem::path& path)
{
  auto impl = std::make_unique<FileHandle::Impl>();
  impl->file_type = FileHandle::eFileType::Midi;
  impl->filepath  = path;
  return FileHandlePtr(new FileHandle(std::move(impl)));
}

} // namespace miniaudioengine
