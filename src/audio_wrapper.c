#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "audio_wrapper.h"
#include "wrapper_utils.h"
#include "complex_numbers.h"


/*
--------------------------------------------------------
        Audio Wrapper Struct Functions and Tools        
--------------------------------------------------------
*/


// Helper function for wrapper initialization
void* _loadMetaData(const char *filename, wavMetaData *wav) {
    // Load file, dump expected header metadata into buffer
    FILE* file = fopen(filename, "rb");
    if (fread(wav, 1, 44, file) != 44) { fclose(file); return NULL; }

    // Metadata optional chunk checking and realignment
    if (memcmp(wav->_subchunk2id, "data", 4) != 0) {
        char checkStr[5] = "bbbb\0";
        uint32_t newSize;

        while (memcmp(checkStr, "data", 4) != 0) {
            memmove(&checkStr[0], &checkStr[1], 4*sizeof(char));
            if (fread(&checkStr[3], 1, 1, file) != 1) { fclose(file); return NULL; }
            checkStr[4] = '\0';
        }

        if (fread(&newSize, 1, 4, file) != 4) { fclose(file); return NULL; }
        strcpy(wav->_subchunk2id, checkStr);
        wav->_datasize = newSize;
    }

    // Data assignment and final return
    size_t floatLen = wav->_datasize;
    void *data = malloc(floatLen);

    size_t resLoad = fread(data, 1, floatLen, file);
    fclose(file);
    if (resLoad != floatLen) { free(data); return NULL; }
    return data;
}


// Raw WAV data to complex-float value conversion function
bool _convertToComplex(complex *carr, void *farr, long len, unsigned int type) {
    switch (type) {
        case 8: { // Single byte precision
            int8_t* f_args = (int8_t*) farr; 
            for (long i = 0; i < len; i++) {
                carr[i].r = (float) f_args[i];
            }
            return true;
        }
        case 16: { // Double byte precision
            int16_t* f_args = (int16_t*) farr; 
            for (long i = 0; i < len; i++) {
                carr[i].r = (float) f_args[i];
            }
            return true;
        }
        case 24:   // 3 & 4 byte precision
        case 32: {
            int32_t* f_args = (int32_t*) farr; 
            for (long i = 0; i < len; i++) {
                carr[i].r = (float) f_args[i];
            }
            return true;
        }
        default:    // Invalid metadata types
            return false;
    }
}


// Prints all data stored inside an AudioWrapper besides the data stream itself
void printWrapperInfo(audioWrapper *awr) {
    printf(
        "Sample rate: %u\n"
        "Frame rate: %u\n"
        "Window size: %u\n"
        "Time length: %f\n"
        "Window count: %u\n"
        "Sample count: %u\n"
        "Window offset: %u\n"
        "Sorted channels: %s\n"
        "Mono: %s\n",
        awr->sampleRate,
        awr->frameRate,
        awr->windowSize,
        awr->lenSeconds,
        awr->numWindows,
        awr->numSamples,
        awr->winOffset,
        awr->sorted ? "true" : "false",
        awr->mono ? "true" : "false"
    );
}


// Prints all required metadata information from a WAV file
void _printWavInfo(wavMetaData *mta) {
    printf(
        "RIFF - %.4s\n"
        "File length (-8b): %u\n"
        "WAVE - %.4s\n"
        "FMT - %.4s\n"
        "S1 Chunk size: %u\n"
        "PCM: %u\n"
        "Channels: %u\n"
        "Sample rate: %u\n"
        "Byte rate: %u\n"
        "Block align: %u\n"
        "Bits per sample: %u\n"
        "DATA - %.4s\n"
        "Data size: %u",
        mta->_chunkid,
        mta->_filesize,
        mta->_format,
        mta->_fmt,
        mta->_subchunk1size,
        mta->_audioformat,
        mta->_numchannels,
        mta->_samplerate,
        mta->_byterate,
        mta->_blockalign,
        mta->_bitspersample,
        mta->_subchunk2id,
        mta->_datasize
    );
}


// Creates, initializes, then returns a new AudioWrapper from the provided arguments
audioWrapper* newAudioWrapper(const char* filename, 
                              unsigned int framerate, 
                              float visualseconds,
                              bool sort,
                              bool display) {
    audioWrapper *n = malloc(sizeof(audioWrapper));
    if (!initWrapper(n, filename, framerate, visualseconds, sort, display)) { return NULL; }
    return n;
}


// Initializes a pre-constructed AudioWrapper using its pointer reference
bool initWrapper(audioWrapper *awr, 
                 const char* filename, 
                 unsigned int framerate, 
                 float visualseconds,
                 bool sort,
                 bool display) {
    // Open file and dump meta-data
    wavMetaData wavData;
    void* data = _loadMetaData(filename, &wavData);
    if (data == NULL) { return false; }       // Ensure data is loaded
    if (display) { _printWavInfo(&wavData); } // Display all dumped info if true

    // Assign based on meta-data
    awr->numSamples = (unsigned int) wavData._datasize / (wavData._numchannels * (wavData._bitspersample / 8));
    awr->sampleRate = (unsigned int) wavData._samplerate;
    awr->frameRate = framerate;
    awr->windowSize = (unsigned int) (wavData._samplerate * visualseconds);
    awr->lenSeconds = ((float) awr->numSamples) / wavData._samplerate;
    awr->numWindows = (unsigned int) (awr->lenSeconds * awr->frameRate);                   
    awr->winOffset = (unsigned int) (((float) (awr->numSamples - awr->windowSize)) / awr->numWindows);
    awr->sorted = false;
    awr->mono = !((bool) (wavData._numchannels - 1));

    // Convert to complex value array (consisting of all reals)
    long dataLen = awr->numSamples * wavData._numchannels;
    complex *comp_array = (complex *) malloc(sizeof(complex) * dataLen);
    if (!_convertToComplex(comp_array, data, dataLen, wavData._bitspersample)) { 
        free(comp_array);
        return false; 
    };

    awr->rawData = comp_array;
    if (sort) { sortChannels(awr); }

    return true;
}


// Clears the memory for a given AudioWrapper and its data
void deleteAudioWrapper(audioWrapper *awr) {
    free(awr->rawData);
    free(awr);
}