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
-----------------------------------------
        Single Frame Processing
-----------------------------------------
*/


complex* dft(complex* data, unsigned int len) {
    complex* dftArr = (complex*) malloc(sizeof(complex) * len);
    
    for (unsigned int k = 0; k < len; k++) {                  // Frequency Range
        complex assign_c = {0.0f, 0.0f};
        for (unsigned int n = 0; n < len; n++) {              // Element Range
            float rot = -(2.0f * M_PI * k * n) / len;         // Twiddle Rotation
            ipaddc(&assign_c, multc(data[n], from_rot(rot)));
        }
        dftArr[k] = assign_c;
    }

    return dftArr;
}

complex* idft(complex* data, unsigned int len) {
    complex* dftArr = (complex*) malloc(sizeof(complex) * len);
    
    for (unsigned int k = 0; k < len; k++) {                  // Frequency Range
        complex assign_c = {0.0f, 0.0f};
        for (unsigned int n = 0; n < len; n++) {              // Element Range
            float rot = (2.0f * M_PI * k * n) / len;          // Inverse Twiddle Rotation
            ipaddc(&assign_c, multc(data[n], from_rot(rot)));
        }
        iprealc(&assign_c);
        dftArr[k].r = assign_c.r / (float) len;
    }

    return dftArr;
}

void _fftProc(complex* data, complex* buff, unsigned int len, bool inverse) {
    if (len == 1) // Base case
        return;

    // In-place decimation
    unsigned int _halfLen = len/2;
    for (unsigned int i=0; i<_halfLen; i++) {
        buff[i] = data[i * 2];
        buff[i + _halfLen] = data[(i * 2) + 1];
    }

    // Split each half further
    _fftProc(buff, data, _halfLen, inverse);
    _fftProc(&buff[_halfLen], data, _halfLen, inverse);

    // Reconstruction
    for (unsigned int i=0; i<_halfLen; i++) {
        float _angle = ((inverse) ? 2 : -2) * M_PI * i / (float) len;
        complex _rot = multc(from_rot(_angle), buff[i + _halfLen]);
        data[i] = addc(buff[i], _rot);
        data[i + _halfLen] = subc(buff[i], _rot);
    }
}

void _cztProc(complex* data, unsigned int len, bool inverse) {
    // Nearest power of 2 array setup (and zero the memory space)
    unsigned int _upscaleLen = (unsigned int) (2 << (int) ceilf(log2f((float) len)));
    complex* _convBase = (complex*) malloc(sizeof(complex) * _upscaleLen);
    complex* _convMult = (complex*) malloc(sizeof(complex) * _upscaleLen);
    memset(_convMult, 0, sizeof(complex) * _upscaleLen);
    memset(_convBase, 0, sizeof(complex) * _upscaleLen);

    // Twiddle preparation and assignment
    complex* _twidFwrd = (complex*) malloc(sizeof(complex) * len);
    complex* _twidBkwd = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int i=0; i<len; i++) {
        float _angle = ((inverse) ? -1 : 1) * M_PI * (float) (i * i) / (float) len;
        _twidFwrd[i] = from_rot(-_angle);
        _twidBkwd[i] = from_rot(_angle);
    }

    // Buffer array preparation
    memcpy(_convMult, _twidFwrd, sizeof(complex) * len);
    free(_twidFwrd);
    for (unsigned int i=1; i<len; i++) {
        _convMult[_upscaleLen - i] = _convMult[i];
        _convBase[i] = multc(data[i], _twidBkwd[i]);
    }

    // Cooley-Tukey Instances & Convolution
    complex* _buff = (complex*) malloc(sizeof(complex) * _upscaleLen);
    _fftProc(_convBase, _buff, _upscaleLen, false);
    memset(_buff, 0, sizeof(complex) * _upscaleLen);
    
    _fftProc(_convMult, _buff, _upscaleLen, false);
    ipmultcArr(_convBase, _convMult, _upscaleLen);
    memset(_buff, 0, sizeof(complex) * _upscaleLen);

    _fftProc(_convBase, _buff, _upscaleLen, true);
    free(_buff);
    free(_convMult);
    
    // Final application
    ipmultcArr(_convBase, _twidBkwd, len);
    ipdivcfArr_s(_convBase, (float) _upscaleLen, len);
    memcpy(data, _convBase, sizeof(complex) * len);
    free(_convBase);
    free(_twidBkwd);
}

complex* fft(complex* data, unsigned int len) {
    if (len < 500)
        return dft(data, len);

    if (!(len & (len - 1))) {
        complex* fftArr = (complex*) malloc(sizeof(complex) * len);
        complex* _buff = (complex*) malloc(sizeof(complex) * len);
        memcpy(fftArr, data, sizeof(complex) * len);

        _fftProc(fftArr, _buff, len, false);
        free(_buff);
        return fftArr;
    }

    complex* fftArr = (complex*) malloc(sizeof(complex) * len);
    memcpy(fftArr, data, sizeof(complex) * len);
    _cztProc(fftArr, len, false);
    return fftArr;
}

complex* ifft(complex* data, unsigned int len) {
    if (len < 500)
        return idft(data, len);

    if (!(len & (len - 1))) {
        complex* ifftArr = (complex*) malloc(sizeof(complex) * len);
        complex* _buff = (complex*) malloc(sizeof(complex) * len);
        memcpy(ifftArr, data, sizeof(complex) * len);

        _fftProc(ifftArr, _buff, len, true);
        free(_buff);
        ipdivcfArr_s(ifftArr, (float) len, len);
        return ifftArr;
    }

    complex* ifftArr = (complex*) malloc(sizeof(complex) * len);
    memcpy(ifftArr, data, sizeof(complex) * len);
    _cztProc(ifftArr, len, true);
    ipdivcfArr_s(ifftArr, (float) len, len);
    return ifftArr;
}

complex* realVals(complex* data, unsigned int len) {
    complex* realArr = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int k = 0; k < len; k++) {
        realArr[k] = realc(data[k]);
    }
    return realArr;
}

complex* compVals(complex* data, unsigned int len) {
    complex* compArr = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int k = 0; k < len; k++) {
        compArr[k] = compc(data[k]);
    }
    return compArr;
}

complex* absVals(complex* data, unsigned int len) {
    complex* absArr = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int k = 0; k < len; k++) {
        absArr[k] = absc(data[k]);
    }
    return absArr;
}

complex* toDecibels(complex* data, unsigned int len) {
    complex* decArr = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int k = 0; k < len; k++) {
        decArr[k] = multcf(logrc(10.0f, data[k]), 20.0f);
    }
    return decArr;
}

complex* fromDecibels(complex* data, unsigned int len) {
    complex* decArr = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int k = 0; k < len; k++) {
        decArr[k] = powrc(10.0f, divcf(data[k], 20.0f));
    }
    return decArr;
}

complex* normalize(complex* data, unsigned int len) {
    complex _min = minimum(data, len);
    complex _max = maximum(data, len);
    complex _dif = subc(_max, _min);
    complex* normArr = (complex*) malloc(sizeof(complex) * len);
    for (unsigned int k = 0; k < len; k++) {
        normArr[k] = divc(subc(data[k], _min), _dif);
    }
    return normArr;
}