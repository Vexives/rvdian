#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "audio_wrapper.h"
#include "complex_numbers.h"
#include "array_utils.h"


/*
--------------------------------------------
        Entire Sound-file Processing
--------------------------------------------
*/


// Returns the main windows of an AudioWrapper's data stream
complex** getWindows(audioWrapper *awr) {
    if (awr->sorted && !awr->mono) {
        complex** windowArr = (complex**) malloc(sizeof(complex*) * awr->numWindows * 2);
        size_t amt = awr->windowSize * sizeof(complex);

        for (size_t i = 0; i < awr->numWindows; i++) {
            // Left channel windows
            windowArr[i] = (complex*) malloc(amt);
            memcpy(windowArr[i], &awr->rawData[i * awr->winOffset], amt);

            // Right channel windows
            windowArr[i + awr->numWindows] = (complex*) malloc(amt);
            memcpy(windowArr[i + awr->numWindows], 
                   &awr->rawData[awr->numSamples + (i * awr->winOffset)], 
                   amt);
        }
        return windowArr;
    }
    else {
        complex** windowArr = (complex**) malloc(sizeof(complex*) * awr->numWindows);
        size_t amt = awr->windowSize * (1 + (unsigned int) !awr->mono) * sizeof(complex);
        size_t offset = awr->winOffset * (1 + (unsigned int) !awr->mono);

        for (size_t i = 0; i < awr->numWindows; i++) {
            windowArr[i] = (complex*) malloc(amt);
            memcpy(windowArr[i], &awr->rawData[i * offset], amt);
        }
        return windowArr;
    }
}


// Clears the memory for all windows in a given data window list
void collapseWindows(audioWrapper *awr, complex** wnds) {
    unsigned int sortStereo = (1 + (unsigned int) (awr->sorted && !awr->mono));
    for (size_t i = 0; i < awr->numWindows * sortStereo; i++) {
        free(wnds[i]);
    }
    free(wnds);
}


// Applies a function (window or frame process) to each frame in a multi-frame list
complex** windowApply(audioWrapper* awr, complex** windows, 
                      complex* (*func)(complex*, unsigned int),
                      bool display) {
    complex** newWindows = (complex**) malloc(sizeof(complex*) * awr->numWindows);

    for (int i=0; i<awr->numWindows; i++) {
        newWindows[i] = func(windows[i], awr->windowSize);
        if (display)
            printf("Window %u render complete.\n", i);
    }
    return newWindows;
}


// Returns a singular channel from a provided AudioWrapper
complex* getChannel(audioWrapper *awr, bool right) {
    if (awr->mono) { return awr->rawData; }
    if (awr->sorted) {
        complex* channel = (complex*) malloc(sizeof(complex) * awr->numSamples);
        unsigned int offset = awr->numSamples * (unsigned int) right;
        memcpy(channel, &awr->rawData[offset], sizeof(complex) * awr->numSamples);
        return channel;
    }
    else {
        complex* channel = (complex*) malloc(sizeof(complex) * awr->numSamples);
        for (long i = (long) right; i < awr->numSamples*2; i += 2) {
            channel[i / 2] = awr->rawData[i];
        }
        return channel;
    }
}


// Sorts an unsorted AudioWrapper's data stream into two channels, unlacing the raw data
void sortChannels(audioWrapper *awr) {
    if (awr->mono || awr->sorted) { return; }
    complex* leftChannel = getChannel(awr, false);
    complex* rightChannel = getChannel(awr, true);

    memcpy(&awr->rawData[0], leftChannel, awr->numSamples * sizeof(complex));
    memcpy(&awr->rawData[awr->numSamples], rightChannel, awr->numSamples * sizeof(complex));

    free(leftChannel);
    free(rightChannel);
    awr->sorted = true;
}


// Re-interlaces an AudioWrapper's data stream back into the standard WAV data format
void interlaceChannels(audioWrapper *awr) {
    if (awr->mono || !awr->sorted) { return; }
    complex* ilChannels = (complex*) malloc(sizeof(complex) * awr->numSamples * 2);

    for (long i = 0; i < awr->numSamples; i++) {
        ilChannels[i * 2] = awr->rawData[i];                         // Left Channel Samples
        ilChannels[1 + (i * 2)] = awr->rawData[awr->numSamples + i]; // Right Channel Samples
    }
    memcpy(awr->rawData, ilChannels, sizeof(complex) * awr->numSamples * 2);
    free(ilChannels);
    awr->sorted = false;
}


// Normalizes the sound file inside of the Audio Wrapper
void normData(audioWrapper* awr) {
    if (awr->mono) {
        complex _max = maximum(awr->rawData, awr->numSamples);
        complex _min = minimum(awr->rawData, awr->numSamples);
        complex _dif = subc(_max, _min);

        for (unsigned int i=0; i<awr->numSamples; i++) {
            ipsubc(&awr->rawData[i], _min);
            ipdivc(&awr->rawData[i], _dif);
        }
        return;
    }

    bool _sortState = awr->sorted;
    unsigned int _halfPoint = awr->numSamples / 2;
    sortChannels(awr);

    // Left channel normalizing
    complex _maxL = maximum(awr->rawData, _halfPoint);
    complex _minL = minimum(awr->rawData, _halfPoint);
    complex _difL = subc(_maxL, _minL);

    for (unsigned int i=0; i<_halfPoint; i++) {
        ipsubc(&awr->rawData[i], _minL);
        ipdivc(&awr->rawData[i], _difL);
    }

    // Right channel normalizing
    complex _maxR = maximum(&awr->rawData[_halfPoint-1], _halfPoint);
    complex _minR = minimum(&awr->rawData[_halfPoint-1], _halfPoint);
    complex _difR = subc(_maxR, _minR);

    for (unsigned int i=_halfPoint-1; i<awr->numSamples; i++) {
        ipsubc(&awr->rawData[i], _minR);
        ipdivc(&awr->rawData[i], _difR);
    }
    
    // Realign
    if (!_sortState)
        interlaceChannels(awr);
    return;
}


// Removes the DC offset from the sound file inside of the Audio Wrapper
void removeDCOffset(audioWrapper* awr) {
    if (awr->mono) {
        complex _dc = mean(awr->rawData, awr->numSamples);

        for (unsigned int i=0; i<awr->numSamples; i++) {
            ipsubc(&awr->rawData[i], _dc);
        }
        return;
    }

    bool _sortState = awr->sorted;
    unsigned int _halfPoint = awr->numSamples / 2;
    sortChannels(awr);

    // Left channel correcting
    complex _dcL = mean(awr->rawData, _halfPoint);
    for (unsigned int i=0; i<_halfPoint; i++) {
        ipsubc(&awr->rawData[i], _dcL);
    }

    // Right channel correcting
    complex _dcR = mean(&awr->rawData[_halfPoint-1], _halfPoint);
    for (unsigned int i=_halfPoint-1; i<awr->numSamples; i++) {
        ipsubc(&awr->rawData[i], _dcR);
    }
    
    // Realign
    if (!_sortState)
        interlaceChannels(awr);
    return;
}

// Monoize

// Stereoize