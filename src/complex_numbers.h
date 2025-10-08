#ifndef COMPLEX_VALUES_H_
#define COMPLEX_VALUES_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Complex Float Numerics
typedef struct compfloat_t {
    float r;
    float i;
} complex;


void ipaddc(complex *dest, complex addit);
void ipaddcf(complex *dest, float addit);
void ipsubc(complex *dest, complex subit);
void ipsubcf(complex *dest, float subit);
void ipmultc(complex *dest, complex multit);
void ipmultcf(complex *dest, float multit);
void ipdivc(complex *dest, complex divit);
void ipdivcf(complex *dest, float divit);
void ipabsc(complex *dest);
void iprealc(complex *dest);
void ipcompc(complex *dest);
void ipmultcArr(complex* array1, complex* array2, unsigned int len);
void ipmultcArr_s(complex* array, complex val, unsigned int len);
void ipmultcfArr_s(complex* array, float val, unsigned int len);
void ipdivcArr(complex* array1, complex* array2, unsigned int len);
void ipdivcArr_s(complex* array, complex val, unsigned int len);
void ipdivcfArr_s(complex* array, float val, unsigned int len);
complex flipc(complex a);
complex flipcr(complex a);
complex flipci(complex a);
complex addc(complex a, complex b);
complex addcf(complex a, float b);
complex subc(complex a, complex b);
complex subcf(complex a, float b);
complex subrc(float a, complex b);
complex multc(complex a, complex b);
complex multcf(complex a, float b);
complex divc(complex a, complex b);
complex divrc(float a, complex b);
complex divcf(complex a, float b);
complex absc(complex a);
float abscf(complex a);
complex expc(complex a);
complex sinc(complex a);
complex cosc(complex a);
complex from_rot(float rot);
complex realc(complex a);
complex compc(complex a);
complex powrc(float a, complex b);
complex powfc(complex a, float b);
complex powc(complex a, complex b);
complex logrc(float base, complex a);
complex logfc(complex base, float a);
complex logc(complex base, complex a);

#endif // COMPLEX_VALUES_H_