#ifndef FRAME_PROCESSES_LIST_H_
#define FRAME_PROCESSES_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "audio_wrapper.h"
#include "array_utils.h"
#include "complex_numbers.h"

complex* dft(complex* data, unsigned int len);
complex* idft(complex* data, unsigned int len);
void _fftProc(complex* data, complex* buff, unsigned int len, bool inverse);
void _cztProc(complex* data, unsigned int len, bool inverse);
complex* fft(complex* data, unsigned int len);
complex* ifft(complex* data, unsigned int len);
complex* realVals(complex* data, unsigned int len);
complex* compVals(complex* data, unsigned int len);
complex* absVals(complex* data, unsigned int len);
complex* toDecibels(complex* data, unsigned int len);
complex* fromDecibels(complex* data, unsigned int len);

#endif // FRAME_PROCESSES_LIST_H_