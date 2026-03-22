#ifndef __WAV_FILE_H__
#define __WAV_FILE_H__

/**
 * @file wavfile.h
 * @deprecated This header is a compatibility stub. Use FileHandle from filehandle.h instead.
 */
#include "filehandle.h"

namespace miniaudioengine
{

/** @deprecated Use FileHandle / FileHandlePtr instead. */
[[deprecated("Use FileHandle from filehandle.h instead of WavFile")]]
typedef FileHandle WavFile;

/** @deprecated Use FileHandlePtr instead. */
[[deprecated("Use FileHandlePtr from filehandle.h instead of WavFilePtr")]]
typedef FileHandlePtr WavFilePtr;

} // namespace miniaudioengine

#endif // __WAV_FILE_H__
