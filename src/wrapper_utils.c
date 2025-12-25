#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "complex_numbers.h"
#include "array_utils.h"
#include "audio_wrapper.h"


/*
--------------------------------------------
        Entire Sound-file Processing
--------------------------------------------
*/


// Applies a function (window or frame process) to each frame in a multi-frame list
complex** windowApply(audioWrapper* awr, complex** windows, 
                      complex* (*func)(complex*, unsigned int),
                      bool display) {
    complex** newWindows = (complex**) malloc(sizeof(complex*) * awr->numWindows);

    for (unsigned int i=0; i<awr->numWindows; i++) {
        newWindows[i] = func(windows[i], awr->windowSize);
        if (display)
            printf("Window %u render complete.\n", i);
    }
    return newWindows;
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
    unsigned int _halfPoint = awr->numSamples;
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
    complex _maxR = maximum(&awr->rawData[_halfPoint], _halfPoint);
    complex _minR = minimum(&awr->rawData[_halfPoint], _halfPoint);
    complex _difR = subc(_maxR, _minR);

    for (unsigned int i=_halfPoint; i<_halfPoint*2; i++) {
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
        for (unsigned int i=0; i<awr->numSamples; i++)
            ipsubc(&awr->rawData[i], _dc);
        return;
    }

    bool _sortState = awr->sorted;
    unsigned int _halfPoint = awr->numSamples;
    sortChannels(awr);

    // Left channel correcting
    complex _dcL = mean(awr->rawData, _halfPoint);
    for (unsigned int i=0; i<_halfPoint; i++)
        ipsubc(&awr->rawData[i], _dcL);

    // Right channel correcting
    complex _dcR = mean(&awr->rawData[_halfPoint], _halfPoint);
    for (unsigned int i=_halfPoint; i<_halfPoint*2; i++)
        ipsubc(&awr->rawData[i], _dcR);
    
    // Realign
    if (!_sortState)
        interlaceChannels(awr);
    return;
}


// Monoize the raw data stored in the Audio Wrapper
void monoize(audioWrapper* awr) {
    if (awr->mono) { return; }
    // Sort and process (average the data between channels)
    sortChannels(awr);
    complex* _channel = (complex*) malloc(sizeof(complex) * awr->numSamples);
    memcpy(_channel, awr->rawData, sizeof(complex) * awr->numSamples);
    ipmultcArr(_channel, &awr->rawData[awr->numSamples], awr->numSamples);
    ipdivcfArr_s(_channel, 2.0f, awr->numSamples);

    // Assign and update
    awr->mono = true;
    free(awr->rawData);
    awr->rawData = _channel;
}


// Stereoize the raw data stored in the Audio Wrapper
void stereoize(audioWrapper* awr) {
    if (!awr->mono) { return; }
    // Create new double-channel audio space
    complex* _channel = (complex*) malloc(sizeof(complex) * awr->numSamples * 2);
    memcpy(_channel, awr->rawData, sizeof(complex) * awr->numSamples);
    memcpy(&_channel[awr->numSamples], awr->rawData, sizeof(complex) * awr->numSamples);

    // Assign and update
    awr->mono = false;
    free(awr->rawData);
    awr->rawData = _channel;
}