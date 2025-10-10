# **RⱯDIAN - Customizable Audio Visualizer**

| [**Information**](#information)
| [**Installation**](#installation)
| [**Notes**](#notes)
| [**Future Plans**](#future-plans-and-developments)
|



A visualizer generator for WAVE format audio files using C.



## Information

> [!NOTE]
> RⱯDIAN is still a work-in-progress. In its current state, it is not usable as intended and will perform differently than the final product. It will be completed in time, and details are subject to change upon a full release. The public repository is meant to keep track and log all changes alongside its development.

RⱯDIAN is a tool that allows for simple audio waveform and Fourier transform type visualizers for WAVE (`.wav`) format files. It allows for various operations to customize the audio data before splitting it into animatable video frames.

Each frame of the audio can be processed using a series of functions which change the look of the resulting waveform. Some of these options include:

- Window functions (Including Hann, Hamming, Flat-Top, and more).
- Fast Fourier Transforms, including prime & non-2-power length frames.
- Isolation of complex components (real and imaginary), as well as absolute values.
- Decimal value conversion and normalization.

All algorithms for complex number calculations and array transforms are written natively and are optimized to prioritize performance and low-memory operations.




## Installation

> [!CAUTION]
> Because a full release is not completed, the project will not function as intended ([**See Above**](#information)). As such, it is not recommended to install this project as of yet. If you still wish to install, you can clone this repository and run the `Makefile` using a Linux compiler ([GCC]) or a Windows compiler ([MinGW]).


## Notes

**More notes and considerations will be included as the program develops further. Stay tuned!**




## Future Plans

 - A command line interface, allowing for quick customization and rendering
 - A full graphical implementation, quickly creating a simple visualizer video for a given audio file.
 - Further customization options and frame processing.
 - Increased algorithmic optimizations where possible.
 - More...




[GCC]: https://gcc.gnu.org/
[MinGW]: https://www.mingw-w64.org/