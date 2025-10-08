#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "complex_numbers.h"

/*
---------------------------------------------
    COMPLEX NUMBER CALCULATION FUNCTIONS
---------------------------------------------
*/

// In-place Complex Addition
void ipaddc(complex *dest, complex addit) {
    dest->r += addit.r;
    dest->i += addit.i;
}

// In-place Complex-to-Float Addition
void ipaddcf(complex *dest, float addit) {
    dest->r += addit;
}

// In-place Complex Subtraction
void ipsubc(complex *dest, complex subit) {
    dest->r -= subit.r;
    dest->i -= subit.i;
}

// In-place Complex-to-Float Subtraction
void ipsubcf(complex *dest, float subit) {
    dest->r -= subit;
}

// In-place Complex Multiplication
void ipmultc(complex *dest, complex multit) {
    complex _buff = (complex) {(dest->r * multit.r) - (dest->i * multit.i), 
                               (dest->r * multit.i) + (dest->i * multit.r)};
    dest->r = _buff.r;
    dest->i = _buff.i;
}

// In-place Complex-to-Float Multiplication
void ipmultcf(complex *dest, float multit) {
    dest->r = dest->r * multit;
    dest->i = dest->i * multit;
}

// In-place Complex Division
void ipdivc(complex *dest, complex divit) {
    float divs = (divit.r * divit.r) + (divit.i * divit.i);
    complex _buff = (complex) {((dest->r * divit.r) + (dest->i * divit.i)) / divs,
                               ((dest->i * divit.r) - (dest->r * divit.i)) / divs};
    dest->r = _buff.r;
    dest->i = _buff.i;
}

// In-place Complex Division
void ipdivcf(complex *dest, float divit) {
    dest->r = dest->r / divit;
    dest->i = dest->i / divit;
}

// In-place Complex Absolute Value (Magnitude)
void ipabsc(complex *dest) {
    dest->r = sqrtf((dest->r * dest->r) + (dest->i * dest->i));
    dest->i = 0.0f;   
}

// In-place Complex Real Values
void iprealc(complex *dest) {
    dest->i = 0.0f;
}

// In-place Complex Imaginary Values
void ipcompc(complex *dest) {
    dest->r = 0.0f;
}

// In-place Array-to-Array Multiplication
void ipmultcArr(complex* array1, complex* array2, unsigned int len) {
    for (unsigned int i=0; i<len; i++) ipmultc(&array1[i], array2[i]);
}

// In-place Array-to-Complex Multiplication
void ipmultcArr_s(complex* array, complex val, unsigned int len) {
    for (unsigned int i=0; i<len; i++) ipmultc(&array[i], val);
}

// In-place Array-to-Float Multiplication
void ipmultcfArr_s(complex* array, float val, unsigned int len) {
    for (unsigned int i=0; i<len; i++) ipmultcf(&array[i], val);
}

// In-place Array-to-Array Division
void ipdivcArr(complex* array1, complex* array2, unsigned int len) {
    for (unsigned int i=0; i<len; i++) ipdivc(&array1[i], array2[i]);
}

// In-place Array-to-Complex Division
void ipdivcArr_s(complex* array, complex val, unsigned int len) {
    for (unsigned int i=0; i<len; i++) ipdivc(&array[i], val);
}

// In-place Array-to-Complex Division
void ipdivcfArr_s(complex* array, float val, unsigned int len) {
    for (unsigned int i=0; i<len; i++) ipdivcf(&array[i], val);
}

// Complex Full-Sign Flip
complex flipc(complex a) {
    complex x;
    x.r = -a.r;
    x.i = -a.i;
    return x;
}

// Complex Real-Sign Flip
complex flipcr(complex a) {
    complex x;
    x.r = -a.r;
    x.i = a.i;
    return x;
}

// Complex Imaginary-Sign Flip
complex flipci(complex a) {
    complex x;
    x.r = a.r;
    x.i = -a.i;
    return x;
}

// Complex Addition
complex addc(complex a, complex b) {
    complex x;
    x.r = a.r + b.r;
    x.i = a.i + b.i;
    return x;
}

// Complex-to-Float Addition
complex addcf(complex a, float b) {
    complex x;
    x.r = a.r + b;
    x.i = a.i;
    return x;
}

// Complex Subtraction
complex subc(complex a, complex b) {
    complex x;
    x.r = a.r - b.r;
    x.i = a.i - b.i;
    return x;
}

// Complex-to-Float Subtraction
complex subcf(complex a, float b) {
    complex x;
    x.r = a.r - b;
    x.i = a.i;
    return x;    
}

// Float-to-Complex Subtraction
complex subrc(float a, complex b) {
    complex x;
    x.r = a - b.r;
    x.i = -b.i;
    return x;
}

// Complex Multiplication
complex multc(complex a, complex b) {
    complex x;
    x.r = (a.r * b.r) - (a.i * b.i);
    x.i = (a.r * b.i) + (a.i * b.r);
    return x;
}

// Complex-to-Float Multiplication
complex multcf(complex a, float b) {
    complex x;
    x.r = a.r * b;
    x.i = a.i * b;
    return x;
}

// Complex Division
complex divc(complex a, complex b) {
    complex x;
    float _divs = (b.r * b.r) + (b.i * b.i);
    x.r = ((a.r * b.r) + (a.i * b.i)) / _divs;
    x.i = ((a.i * b.r) - (a.r * b.i)) / _divs;
    return x;
}

// Float (Real)-to-Complex Division
complex divrc(float a, complex b) {
    complex x;
    float _divs = (b.r * b.r) + (b.i * b.i);
    x.r = (a * b.r) / _divs;
    x.i = (a * b.i) / _divs;
    return x;
}

// Complex-to-Float Division
complex divcf(complex a, float b) {
    complex x;
    x.r = a.r / b;
    x.i = a.i / b;
    return x;    
}

// Complex Absolute Value (Magnitude)
complex absc(complex a) {
    complex x;
    x.r = sqrtf((a.r * a.r) + (a.i * a.i));
    return x;
}

// Complex Absolute Value (Magnitude) Float Conversion
float abscf(complex a) {
    return sqrtf((a.r * a.r) + (a.i * a.i));
}

// Complex Exponential Function
complex expc(complex a) {
    complex x;
    float _rexp = expf(a.r);
    x.r = cosf(a.i) * _rexp;
    x.i = sinf(a.i) * _rexp;
    return x;
}

// Complex Sine Function
complex sinc(complex a) {
    complex x;
    x.r = sinf(a.r) * coshf(a.i);
    x.i = cosf(a.r) * sinhf(a.i);
    return x;
}

// Complex Cosine Function
complex cosc(complex a) {
    complex x;
    x.r = cosf(a.r) * coshf(a.i);
    x.i = sinf(a.r) * sinhf(a.i);
    return x;
}

// Rotation to Complex
complex from_rot(float rot) {
    complex x;
    x.r = cosf(rot);
    x.i = sinf(rot);
    return x;
}

// Complex Real Values
complex realc(complex a) {
    complex x;
    x.r = a.r;
    return x;
}

// Complex Imaginary Values
complex compc(complex a) {
    complex x;
    x.i = a.i;
    return x;
}

// Float (Real)-to-Complex Power Function
complex powrc(float a, complex b) {
    complex x;
    float _factor = powf(a, b.r);
    x.r = cosf(b.i * logf(a)) * _factor;
    x.i = sinf(b.i * logf(a)) * _factor;
    return x;
}

// Complex-to-Float Power Function
complex powfc(complex a, float b) {
    complex x;
    float _deg = atanf(a.i / a.r);
    float _rad = powf(abscf(a), b);
    x.r = cosf(b * _deg) * _rad;
    x.i = sinf(b * _deg) * _rad;
    return x;
}

// Complex Power Function
complex powc(complex a, complex b) {
    float _deg = atanf(a.i / a.r);
    float _rad = logf(abscf(a));
    return expc(multc(b, (complex) {_rad, _deg}));
}

// Float (Real)-to-Complex Any Base Logarithm
complex logrc(float base, complex a) {
    float _nb = logf(base);
    float _deg = atanf(a.i / a.r);
    float _rad = logf(abscf(a));
    return divcf((complex) {_rad, _deg}, _nb);
}

// Complex-to-Float Any Base Logarithm
complex logfc(complex base, float a) {
    float _na = logf(a);
    float _deg = atanf(base.i / base.r);
    float _rad = logf(abscf(base));
    return divrc(_na, (complex) {_rad, _deg});
}

// Complex Any Base Logarithm
complex logc(complex base, complex a) {
    float _degA = atanf(a.i / a.r);
    float _radA = logf(abscf(a));
    float _degB = atanf(base.i / base.r);
    float _radB = logf(abscf(base));
    return divc((complex) {_radA, _degA}, (complex) {_radB, _degB});
}