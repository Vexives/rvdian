#ifndef AUDIO_WRAPPER_H_
#define AUDIO_WRAPPER_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "wrapper_utils.h"
#include "complex_numbers.h"


// Audio-Wrapper Struct Def
typedef struct wavsampler_t {
    unsigned int sampleRate;
    unsigned int frameRate;
    unsigned int windowSize;
    float lenSeconds;
    unsigned int numWindows;
    unsigned int numSamples;
    unsigned int winOffset;
    bool mono;
    bool sorted;
    complex *rawData;
} audioWrapper;


// WAV File Header Metadata Dump-Struct Def
typedef struct wavmeta_t {
    char _chunkid[4];         // RIFF subchunk (without null char)
    uint32_t _filesize;       // File length - 8 bytes
    char _format[4];          // WAVE (without null char)

    char _fmt[4];             // FMT subchunk (without null char)
    uint32_t _subchunk1size;  // 16 or 0x10

    uint16_t _audioformat;    // PCM
    uint16_t _numchannels;    // Channels (mono/stereo)

    uint32_t _samplerate;     // Sample rate
    uint32_t _byterate;       // Sample rate * num channels * bits-per-sample / 8
    
    uint16_t _blockalign;     // Num channels * bits-per-sample / 8
    uint16_t _bitspersample;  // 8 or 16 bit

    char _subchunk2id[4];     // DATA subchunk (without null char)
    uint32_t _datasize;       // Data-size
} wavMetaData;


void* _loadMetaData(const char *filename, wavMetaData *wav);
bool _convertToComplex(complex *carr, void *farr, long len, unsigned int type);
void printWrapperInfo(audioWrapper *awr);
void _printWavInfo(wavMetaData *mta);
audioWrapper *newAudioWrapper(const char* filename, 
                              unsigned int framerate, 
                              float visualseconds,
                              bool sort,
                              bool display);
bool initWrapper(audioWrapper *awr, 
                 const char* filename, 
                 unsigned int framerate, 
                 float visualseconds,
                 bool sort,
                 bool display);
void deleteAudioWrapper(audioWrapper *awr);

#endif // AUDIO_WRAPPER_H_