#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "complex_numbers.h"
#include "audio_wrapper.h"
#include "window_funcs.h"
#include "array_utils.h"
#include "wrapper_utils.h"
#include "array_utils.h"

/*
__________________________________________________________________________________

        TEMPORARY TEST MAIN. ENSURING FUNCTIONALITY BEFORE FINAL ASSEMBLY.
__________________________________________________________________________________

*/

int main() {
    printf("Starting file load...\n");
    audioWrapper *awrd = newAudioWrapper("../sounds/testwav.wav", 60, /*(16384.0f / 44100.0f)*/0.2f, true, true);
    printf("\n\nComplete.\n");

    //char hL[5] = "aaaab";
    //memmove(&hL[0], &hL[1], 4*sizeof(char));
    //hL[4] = '\0';
    //printf("%s\n", hL);
    //printf("%c\n", hL[4]);

    if (awrd == NULL) {
        printf("Null pointer.");
        return 1;
    }

    printWrapperInfo(awrd);

    //interlaceChannels(awrd);
    //removeDCOffset(awrd);
    //normData(awrd);

    complex** windows = getWindows(awrd);
    //complex** fftWinds = windowApply(awrd, windows, dft, true);
    //complex* wdftone = /*fftWinds[25];*/ fft(windows[25], awrd->windowSize);
    complex* wdftone = hann(windows[25], awrd->windowSize);
    //complex* _BUFF1 = fft(windows[25], awrd->windowSize);
    //complex* wdftone = fft(wdftonePRE, awrd->windowSize);
    //complex* wdftone = ifft(wdftonePRE, awrd->windowSize);
    FILE* writeF = fopen("../sounds/windowtest.txt", "w+");
    for (long i = 0; i < //32768; i++) {
         awrd->windowSize; i++) {
        if (wdftone == NULL) {
            printf("NULL, BREAK\n");
            break;
        }
        fprintf(writeF, "%f ", wdftone[i].r);
    }
    fclose(writeF);

    //collapseWindows(awrd, fftWinds);
    collapseWindows(awrd, windows);
    free(wdftone);

    //sortChannels(awrd);
    //interlaceChannels(awrd);

    
    //complex* leftc = getChannel(awrd, false);
    //FILE* writeF = fopen("testdata.txt", "w+");
    //for (long i = 0; i < awrd->numSamples; i++) {
    //    fprintf(writeF, "%f ", leftc[i].r);
    //}
    //fclose(writeF);

    //float hpow = log2f(awrd->windowSize);
    //int rup_pow = (int) ceilf(hpow);
    //int true_pow = 2 << (rup_pow - 1);

    //printf("\nNext 2-Pow: %u\nOriginal: %u\n", (int) powf(2, ceilf((int) log2(7000)+1)), 7000);
    //printf("Window Size: %u\nClosest 2-Power: %i", awrd->windowSize, true_pow);

    deleteAudioWrapper(awrd);
    return 0;
}