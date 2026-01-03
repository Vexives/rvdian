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

#define nw_assert(msg, func) if(!(func)) { std::cerr << "STOPPED: " << msg << std::endl; return 1; }

std::unordered_map<std::string, std::function<complex*(complex*, unsigned int)>> _frameFuncs;
std::unordered_map<std::string, std::function<void(audioWrapper*)>> _wrapperFuncs;


std::string _printComplexArray(complex* arr, unsigned int len, bool floats = true) {
    std::ostringstream _buffStr;
    for (unsigned int i=0; i<len; i++) {
        _buffStr << arr[i].r;
        if (!floats)
            _buffStr << "+" << arr[i].i << "j";
        _buffStr << ",";
    }
    _buffStr << endl;
    std::string _retStr = _buffStr.str();
    return _retStr.substr(0, _retStr.size()-2);
}


bool _typeCheck(std::string const& location, std::string const& type) {
    return (location.compare(location.length() - type.length(), type.length(), type)==0);
}


void _setupMaps() {
    // Per-frame & window functions
    _frameFuncs.insert({
        // Standard functions
        {"--dft", dft},
        {"--idft", idft},
        {"--fft", fft},
        {"--ifft", ifft},
        {"--fftshift", fftshift},
        {"--reals", realVals},
        {"--imags", compVals},
        {"--abs", absVals},
        {"--todecibels", toDecibels},
        {"--fromdecibels", fromDecibels},
        {"--norm", normalize},
        // Window functions
        {"--hamming", hamming},
        {"--hann", hann},
        {"--poisson", poisson},
        {"--barlett", barlett},
        {"--lanczos", lanczos},
        {"--tukey", tukey},
        {"--cosine", cosfilter},
        {"--flattop", flattop}
    });

    // Wrapper functions
    _wrapperFuncs.insert({
        {"+normdata", normData},
        {"+removedc", removeDCOffset},
        {"+monoize", monoize},
        {"+stereoize", stereoize}
    });
}


void _displayHelp() {
    _setupMaps();
    std::cout << "RVDIAN Syntax:\n./rvdian <Audiopath> <Framerate> <Height> <Width> <DPI> <Frame length> "
                 "[--effects] [+preprocesses]\nPer-frame Effect Flags:" << std::endl;
    std::ostringstream _effects;
    // Per-frames (Sorted by alphabetical order)
    std::vector<std::string> _oFrames;
    _oFrames.reserve(_frameFuncs.size());
    for (const auto& key : _frameFuncs)
        _oFrames.push_back(key.first);
    std::sort(_oFrames.begin(), _oFrames.end());

    for (const auto& kv : _oFrames)
        _effects << kv << ", ";
    std::string _out = _effects.str();
    std::cout << _out.substr(0, _out.size()-2) << "\nPre-Process Flags:" << std::endl;
    _effects.str("");

    // Pre-processes (Sorted by alphabetical order)
    std::vector<std::string> _oWraps;
    _oWraps.reserve(_wrapperFuncs.size()+2);
    for (const auto& key : _wrapperFuncs)
        _oWraps.push_back(key.first);
    _oWraps.push_back("+display");
    _oWraps.push_back("+logscale");
    std::sort(_oWraps.begin(), _oWraps.end());

    for (const auto& kv : _oWraps)
        _effects << kv << ", ";
    _out = _effects.str();
    std::cout << _out.substr(0, _out.size()-2) << std::endl;
    _effects.clear();
}


// TODO: Something here isn't right.
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
    // Help and info
    if (!strcmp(argv[1], "-help")) {
        _displayHelp();
        return 0;
    }

    // Mandatory file and type checks
    nw_assert("Pathname, framerate, height, width, DPI, and frame length must be specified.", argc >= 7);

    std::vector<std::string> userArgs(argv, argv+argc);
    std::string audioPath = userArgs[1];
    nw_assert("Pathname was not found as a valid file.", fs::exists(audioPath));
    nw_assert("Pathname is not a valid WAVE file.", _typeCheck(argv[1], ".wav"));

    int frameRate = atoi(userArgs[2].c_str());
    nw_assert("Framerate must be an integer greater than 0.", frameRate > 0);

    int frameHeight = atoi(userArgs[3].c_str()), frameWidth = atoi(userArgs[4].c_str());
    nw_assert("Height must be an integer greater than 0.", frameHeight > 0);
    nw_assert("Width must be an integer greater than 0.", frameWidth > 0);

    int dpi = atoi(userArgs[5].c_str());
    nw_assert("DPI must be an integer greater than 0.", dpi > 0);

    float frameLen = atof(userArgs[6].c_str());
    nw_assert("Frame length must be a float greater than 0.0.", frameLen > 0.0f);
    _setupMaps();

    // Process flags and function caches
    std::list<std::function<complex*(complex*, unsigned int)>> frameCache;
    std::list<std::function<void(audioWrapper*)>> wrapCache;
    bool _display = false;
    bool _logscale = false;
    bool _fourier = false;
    for (int i=7; i < argc; i++) {
        if (!userArgs[i].compare("--fft") || !userArgs[i].compare("--dft")) _fourier = true;
        if (!userArgs[i].compare("--ifft") || !userArgs[i].compare("--idft")) _fourier = false;

        if (!userArgs[i].compare("+display")) { _display = true; continue; }
        if (!userArgs[i].compare("+logscale")) { _logscale = true; continue; }
        if (_frameFuncs.count(userArgs[i]) > 0) { frameCache.push_back(_frameFuncs[userArgs[i]]); continue; }
        if (_wrapperFuncs.count(userArgs[i]) > 0) { wrapCache.push_back(_wrapperFuncs[userArgs[i]]); continue; }
        std::cout << "Process \"" << userArgs[i] << "\" not found. Ignoring." << std::endl;
    }

    // Prepare the information dump file and process the Audio Wrapper
    std::ofstream dumpFile("_Frame_DUMP.rvdn");
    audioWrapper* awr = newAudioWrapper(audioPath.c_str(), frameRate, frameLen, true, _display);
    nw_assert("Frame size must be less than or equal to the length of the full audio.", awr->windowSize <= awr->numSamples);
    for (const auto& func : wrapCache) func(awr);

    // Header information
    complex* _freqs = _fourier ? fftfreq(awr->windowSize, 1.0f / awr->sampleRate) : 
                                 linspace((complex){0.0f, 0.0f}, (complex){(float) awr->windowSize, 0.0f}, awr->windowSize, false);
    dumpFile << audioPath << "," << frameHeight << "," << frameWidth << "," << dpi << ",";
    dumpFile << awr->mono << "," << awr->numWindows << "," << frameRate << "," << (_logscale ? "semilogx":"linear") << std::endl;
    dumpFile << _printComplexArray(_freqs, awr->windowSize) << std::endl;
    free(_freqs);

    // If all prior checks are passed, open the frame view and process each frame
    frameView* fv = newFrameView(awr);
    bool _continue = true;
    while (_continue) {
        complex* _left = _aggregateFunctions(fv->frameL, awr->windowSize, frameCache);
        dumpFile << _printComplexArray(_left, awr->windowSize) << std::endl;
        if (!awr->mono) {
            complex* _right = _aggregateFunctions(fv->frameR, awr->windowSize, frameCache);
            dumpFile << _printComplexArray(_right, awr->windowSize) << std::endl;
        }
        _continue = moveFrameForward(awr, fv);
    }

    dumpFile.close();
    deleteFrameView(fv);
    deleteAudioWrapper(awr);

    // TODO:
    // Run the renderer script here (any method works, as long as it's portable)
}