#include <map>
#include <list>
#include <set>
#include <string>
#include <cassert>
#include <functional>
#include <filesystem> //C++17
#include <iostream>
#include <fstream>
using namespace std;
namespace fs = std::filesystem;

extern "C" {
    #include "complex_numbers.h"
    #include "audio_wrapper.h"
    #include "window_funcs.h"
    #include "array_utils.h"
    #include "wrapper_utils.h"
    #include "array_utils.h"
    #include "frame_processes.h"
}

#define nw_assert(func, msg) assert(((void)(msg), (func)))

std::unordered_map<std::string, std::function<complex*(complex*, unsigned int)>> _frameFuncs;
std::unordered_map<std::string, std::function<void(audioWrapper*)>> _wrapperFuncs;

std::string _printComplexArray(complex* arr, unsigned int len, bool floats = true) {
    std::ostringstream _str;
    for (unsigned int i=0; i<len; i++) {
        _str << arr[i].r;
        if (floats)
            _str << "+" << arr[i].i << "j";
        _str << ",";
    }
    return _str.str();
}

bool _typeCheck(std::string const& location, std::string const& type) {
    return (location.compare(location.length() - type.length(), type.length(), type)==0);
}

void _setupMaps() {
    // Per-frame & window functions
    _frameFuncs.insert({
        // Standard functions
        {"dft", dft},
        {"idft", idft},
        {"fft", fft},
        {"ifft", ifft},
        {"fftshift", fftshift},
        {"reals", realVals},
        {"imags", compVals},
        {"abs", absVals},
        {"todecibels", toDecibels},
        {"fromdecibels", fromDecibels},
        // Window functions
        {"norm", normalize},
        {"hamming", hamming},
        {"hann", hann},
        {"poisson", poisson},
        {"barlett", barlett},
        {"lanczos", lanczos},
        {"tukey", tukey},
        {"cosine", cosfilter},
        {"flattop", flattop}
    });

    // Wrapper functions
    _wrapperFuncs.insert({
        {"normdata", normData},
        {"removedc", removeDCOffset},
        {"monoize", monoize},
        {"stereoize", stereoize}
    });
}

complex* _aggregateFunctions(complex* frame, unsigned int len,
                             std::list<std::function<complex*(complex*, unsigned int)>> funcs) {
    complex* agg = frame;
    for (auto const& f : funcs) {
        complex* temp = f(agg, len);
        free(agg);
        agg = temp;
    }
    return agg;
}

int main(int argc, char** argv) {
    // Mandatory file and type checks
    nw_assert("Pathname, framerate, height, width, DPI, and frame length must be specified.", argc >= 7);

    std::string audioPath = argv[1];
    nw_assert("Pathname was not found as a valid file.", fs::exists(audioPath));
    nw_assert("Pathname is not a valid WAVE file.", _typeCheck(argv[1], ".wav"));

    int frameRate = std::atoi(argv[2]);
    nw_assert("Framerate must be an integer greater than 0.", frameRate > 0);

    int frameHeight = std::atoi(argv[3]), frameWidth = std::atoi(argv[4]);
    nw_assert("Height must be an integer greater than 0.", frameHeight > 0);
    nw_assert("Width must be an integer greater than 0.", frameWidth > 0);

    int dpi = std::atoi(argv[5]);
    nw_assert("DPI must be an integer greater than 0.", dpi > 0);

    float frameLen = std::atof(argv[6]);
    nw_assert("Frame length must be a float greater than 0.0.", frameLen > 0.0f);
    _setupMaps();

    // Process flags and function caches
    std::list<std::function<complex*(complex*, unsigned int)>> frameCache;
    //bool _display = false;   TODO: This is if the user wishes to display the Wrapper stats.
    for (int i=7; i < argc; i++) {
        // TODO:
        // If frame function, add to cache and continue.
        // Otherwise, test if it is Audio Wrapper function.
        // If function is not found in either map directories, throw an exception.
    }

    // TODO:
    // Initialize Audio Wrapper and corresponding Frame View.
    // In sequence, iterate Frame View and apply cached functions, write to DUMP file.
    // Call render script and return.
    std::ofstream dumpFile("_Frame_DUMP.rvdn");
    audioWrapper* awr = newAudioWrapper(audioPath.c_str(), frameRate, frameLen, true, true);
    frameView* fv = newFrameView(awr);

    bool _continue = true;
    while (_continue) {
        complex* _left = _aggregateFunctions(fv->frameL, awr->windowSize, frameCache);
        dumpFile << _printComplexArray(_left, frameLen) << "\n";
        if (!awr->mono) {
            complex* _right = _aggregateFunctions(fv->frameR, awr->windowSize, frameCache);
            dumpFile << _printComplexArray(_right, frameLen) << "\n";
        }
        dumpFile << std::flush;
        _continue = moveFrameForward(awr, fv);
    }

    dumpFile.close();
    deleteFrameView(fv);
    deleteAudioWrapper(awr);
}