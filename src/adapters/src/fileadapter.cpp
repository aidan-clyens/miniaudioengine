#include "fileadapter.h"

using namespace miniaudioengine::adapters;


bool FileAdapter::open(const char *filename)
{
  m_info = {};
  p_file = std::shared_ptr<SndFile>(
      sf_open(filename, SFM_READ, &m_info),
      [](SndFile *f)
      { if (f) sf_close(f); });

  if (!p_file)
  {
    return false;
  }

  return true;
}

void FileAdapter::close()
{
  p_file.reset();
}

bool FileAdapter::open_audio_stream(FilePtr file)
{
  // TODO - Implement FileAdapter::open_audio_stream()
  return false;
}

bool FileAdapter::open_midi_stream(FilePtr file)
{
  // TODO - Implement FileAdapter::open_midi_stream()
  return false;
}
