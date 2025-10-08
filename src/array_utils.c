#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "array_utils.h"
#include "audio_wrapper.h"
#include "complex_numbers.h"
#define M_PI 3.14159265358979323846

/*
-----------------------------------------
        Universal Array Utilities             
-----------------------------------------
*/

complex maximum(complex* data, unsigned int len) {
    complex _max = {-INFINITY, 0.0f};
    for (unsigned int i=0; i<len; i++) {
        if (absc(data[i]).r > absc(_max).r)
            _max = data[i];
    }
    return _max;
}

complex minimum(complex* data, unsigned int len) {
    complex _min = {INFINITY, 0.0f};
    for (unsigned int i=0; i<len; i++) {
        if (absc(data[i]).r < absc(_min).r)
            _min = data[i];
    }
    return _min;
}

complex mean(complex* data, unsigned int len) {
    complex _mean = {0.0f, 0.0f};
    for (unsigned int i=0; i<len; i++) {
        ipaddc(&_mean, data[i]);
    }
    ipdivc(&_mean, (complex) {len, 0.0f});
    return _mean;
}

complex* linspace(complex start, complex stop, unsigned int len, bool useEnd) {
    complex* _lspace = (complex*) malloc(sizeof(complex) * len);
    float _rInv = (stop.r - start.r) / (len-1 + (unsigned int) !useEnd);
    float _cInv = (stop.i - start.i) / (len-1 + (unsigned int) !useEnd);

    for (unsigned int i=0; i<len; i++)
        _lspace[i] = (complex) {start.r + (_rInv * i), start.i + (_cInv * i)};
    return _lspace;
}

complex* arange(unsigned int len) {
    complex* _lspace = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int i=0; i<len; i++) _lspace[i] = (complex) {(float) i, 0.0f};
    return _lspace;
}