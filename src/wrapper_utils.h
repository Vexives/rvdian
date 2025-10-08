#ifndef WRAPPER_UTIL_LIST_H_
#define WRAPPER_UTIL_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "audio_wrapper.h"
#include "complex_numbers.h"
#include "array_utils.h"

complex** getWindows(audioWrapper *awr);
void collapseWindows(audioWrapper *awr, complex** wnds);
complex** windowApply(audioWrapper* awr, complex** windows, 
                      complex* (*func)(complex*, unsigned int),
                      bool display);

complex* getChannel(audioWrapper *awr, bool right);
void sortChannels(audioWrapper *awr);
void interlaceChannels(audioWrapper *awr);
void normData(audioWrapper* awr);
void removeDCOffset(audioWrapper* awr);

#endif // WRAPPER_UTIL_LIST_H_