#ifndef AUDIO_UTILS_LIST_H_
#define AUDIO_UTILS_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "audio_wrapper.h"
#include "complex_numbers.h"

complex maximum(complex* data, unsigned int len);
complex minimum(complex* data, unsigned int len);
complex mean(complex* data, unsigned int len);
complex* linspace(complex start, complex stop, unsigned int len, bool useEnd);
complex* arange(unsigned int len);

#endif // AUDIO_UTILS_LIST_H_