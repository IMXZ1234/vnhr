# vnhr
Runtime Hyper-Resolution (HR) for window image outputs, intended for visual novels (VNs), which general involves a limited number of static pictures thoughout gameplay and therefore long inference time of hyper-resolution models can be tolerated to some extent at runtime. 

For Windows OS only, built with Visual Studios Community 2019.
# Functionality
Creates a delegate window for target VN's window, which captures image output of the target window, runs hyper-resolution on-the-fly, and then displays the result image on its own client area. Also, the delegate window relays messages to its target window, making you feel like playing a higher resolution version of that VN.

Image latency and discrepancy between image and sound is unavoidable, but may be alleviated by caching (if can be realized) since number of CGs is actually limited.

You may need a powerful enough GPU to run hyper-resolution at the first place, or to get satisfactory experience.
# Status
Currently, to run you need to do some alterations to the source. You need to download releases of [FFmpeg](https://ffmpeg.org/download.html#build-windows) and [Real-ESRGAN](https://github.com/xinntao/Real-ESRGAN/releases) yourself and configure `szHRExePath, szffmpegExePath` in `realesrganhrmodel.cpp` respectively to their executables' paths and directories.

Under heavy development, planned features:
 - [x] basic function (click and run, with Real-ESRGAN model)
 - [ ] auto mode
 - [ ] config
 - [ ] beautiful icons
 - [ ] deep integration with models (currently through commandline)
 - [ ] more model choices
 - [ ] Direct2D support for image capturing to deal with special cases where GDI fails
 - [ ] opencv integration for image scaling/segmentation (see below)
 - [ ] VN specific image cache/database -- image segmentation into parts (background, character, dialogue...), search in cache and recombine HR parts for display
 - [ ] pre-gen with [GARbro](https://github.com/morkt/GARbro) integration (perhaps another solution)
