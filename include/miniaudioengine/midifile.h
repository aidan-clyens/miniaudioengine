#ifndef __MIDI_FILE_H__
#define __MIDI_FILE_H__

/**
 * @file midifile.h
 * @deprecated This header is a compatibility stub. Use FileHandle from filehandle.h instead.
 */
#include "filehandle.h"

namespace miniaudioengine
{

/** @deprecated Use FileHandle / FileHandlePtr instead. */
[[deprecated("Use FileHandle from filehandle.h instead of MidiFile")]]
typedef FileHandle MidiFile;

/** @deprecated Use FileHandlePtr instead. */
[[deprecated("Use FileHandlePtr from filehandle.h instead of MidiFilePtr")]]
typedef FileHandlePtr MidiFilePtr;

} // namespace miniaudioengine

#endif // __MIDI_FILE_H__