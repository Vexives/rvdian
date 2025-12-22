#include <map>
#include <list>
#include <set>
#include <string>
#include <cassert>
#include <functional>
#include <filesystem> //C++17
#include <iostream>
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

std::unordered_map<std::string, std::function<complex*(complex*, unsigned int)>> _frameFuncs;
std::unordered_map<std::string, std::function<void(audioWrapper*)>> _wrapperFuncs;

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

int main(int argc, char** argv) {
    // Mandatory file and type checks
    assert(("Pathname, framerate, height, width, DPI, and frame length must be specified.", argc >= 7));

    std::string audioPath = argv[1];
    assert(("Pathname \""s + argv[1] + "\" was not found as a valid file.", fs::exists(audioPath)));

    int frameRate = std::atoi(argv[2]);
    assert(("Framerate must be an integer greater than 0.", frameRate > 0));

    int frameHeight = std::atoi(argv[3]), frameWidth = std::atoi(argv[4]);
    assert(("Height must be an integer greater than 0.", frameHeight > 0));
    assert(("Width must be an integer greater than 0.", frameWidth > 0));

    int dpi = std::atoi(argv[5]);
    assert(("DPI must be an integer greater than 0.", dpi > 0));

    float frameLen = std::atof(argv[6]);
    assert(("Frame length must be a float greater than 0.0.", frameLen > 0.0f));
    _setupMaps();

    // Process flags and function caches
    std::list<std::function<complex*(complex*, unsigned int)>> frameCache;
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
}