#ifndef WINDOW_FUNCTIONS_LIST_H_
#define WINDOW_FUNCTIONS_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "array_utils.h"
#include "audio_wrapper.h"
#include "complex_numbers.h"

complex* hamming(complex* data, unsigned int len);
complex* hann(complex* data, unsigned int len);
complex* poisson(complex* data, unsigned int len);
complex* barlett(complex* data, unsigned int len);
complex* lanczos(complex* data, unsigned int len);
complex* tukey(complex* data, unsigned int len);
complex* cosfilter(complex* data, unsigned int len);
complex* flattop(complex* data, unsigned int len);

#endif // WINDOW_FUNCTIONS_LIST_H_