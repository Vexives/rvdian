#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "audio_wrapper.h"
#include "array_utils.h"
#include "complex_numbers.h"
#define M_PI 3.14159265358979323846


/*
--------------------------------------------
        Frame-Usage Window Functions        
--------------------------------------------
*/


complex* hamming(complex* data, unsigned int len) {
    complex* multSpace = linspace((complex) {0.0f, 0.0f}, (complex) {2*M_PI, 0.0f}, len, false);
    for (unsigned int i=0; i<len; i++)
        multSpace[i] = subrc(0.54f, multcf(cosc(multSpace[i]), 0.46f));
    ipmultcArr(multSpace, data, len);
    return multSpace;
}

complex* hann(complex* data, unsigned int len) {
    complex* multSpace = linspace((complex) {0.0f, 0.0f}, (complex) {2*M_PI, 0.0f}, len, false);
    for (unsigned int i=0; i<len; i++)
        multSpace[i] = multcf(subrc(1.0f, cosc(multSpace[i])), 0.5f);
    ipmultcArr(multSpace, data, len);
    return multSpace;
}

complex* poisson(complex* data, unsigned int len) {
    complex* multSpace = arange(len);
    float _cen = ((float) len - 1.0f) / 2.0f;
    float _tau = (_cen * 8.69f) / 60.f;
    for (unsigned int i=0; i<len; i++) {
        complex _temp = divcf(flipc(absc(subcf(multSpace[i], _cen))), _tau);
        multSpace[i] = expc(_temp);
    }
    ipmultcArr(multSpace, data, len);
    return multSpace;
}

complex* barlett(complex* data, unsigned int len) {
    complex* multSpace = arange(len);
    float _cen = (float) (len - 1) / 2.0f;
    float _inv = 2.0f / (float) (len - 1);
    for (unsigned int i=0; i<len; i++) {
        complex _tmp = subrc(_cen, absc(subcf(multSpace[i], _cen)));
        multSpace[i] = multcf(_tmp, _inv);
    }
    ipmultcArr(multSpace, data, len);
    return multSpace;
}

complex* lanczos(complex* data, unsigned int len) {
    complex* multSpace = arange(len);
    for (unsigned int i=0; i<len; i++) {
        complex _tmp = divcf(multcf(multSpace[i], 2.0f), (float) len-1);
        complex _coef = multcf(subcf(_tmp, 1.0f), M_PI);
        multSpace[i] = divc(sinc(_coef), _coef);
    }
    ipmultcArr(multSpace, data, len);
    return multSpace;
}

complex* tukey(complex* data, unsigned int len) {
    complex* multSpace = arange(len);
    float _lowB = (float) len / 4.0f;
    float _highB = (float) len - _lowB;
    for (unsigned int i=0; i<len; i++) {
        if (multSpace[i].r < _lowB) {
            complex _tmp = divcf(multcf(multSpace[i], 4*M_PI), (float) len);
            multSpace[i] = multcf(subrc(1.0f, cosc(_tmp)), 0.5f);
            continue;
        }
        else if (multSpace[i].r > _highB) {
            multSpace[i] = multSpace[len - (i+1)]; 
            continue;
        }
        multSpace[i] = (complex) {1.0f, 0.0f};
    }
    ipmultcArr(multSpace, data, len);
    return multSpace;
}

complex* cosfilter(complex* data, unsigned int len) {
    complex* multSpace = linspace((complex) {-M_PI / 2.0f, 0.0f}, (complex) {M_PI / 2.0f, 0.0f}, len, true);
    for (unsigned int i=0; i<len; i++)
        multSpace[i] = multc(data[i], cosc(multSpace[i]));
    return multSpace;
}

complex* flattop(complex* data, unsigned int len) {
    complex* multSpace = linspace((complex) {0.0f, 0.0f}, (complex) {2*M_PI, 0.0f}, len, false);
    float _consts[5] = {0.21557895f, -0.41663158f, 0.277263158f, -0.083578947f, 0.006947368};
    for (unsigned int i=0; i<len; i++) {
        complex _tmpOne = addc(multcf(cosc(multSpace[i]), _consts[1]),
                               multcf(cosc(multcf(multSpace[i], 2.0f)), _consts[2]));
        complex _tmpTwo = addc(multcf(cosc(multcf(multSpace[i], 3.0f)), _consts[3]),
                               multcf(cosc(multcf(multSpace[i], 4.0f)), _consts[4]));
        multSpace[i] = addc(addcf(_tmpOne, _consts[0]), _tmpTwo);
    }
    ipmultcArr(multSpace, data, len);
    return multSpace;
}