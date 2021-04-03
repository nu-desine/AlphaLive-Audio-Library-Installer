# AlphaLive Audio Library Installer #

AlphaLive Audio Library Installer is a simple GUI utility application that installs the full audio library for [AlphaLive](http://www.alphasphere.com/alphalive/), the [AlphaSphere](http://www.alphasphere.com/)'s official software application.

AlphaLive Audio Library Installer has been programmed using the [JUCE](https://juce.com) cross-platform C++ library. It includes an Xcode project for building on macOS, and a Visual Studio 2019 project for building on Windows. JUCE also easily allows for Linux makefile's to be built.

## Build Requirements ##

To build AlphaLive Audio Library Installer on macOS you will need to download and install [Xcode](https://developer.apple.com/xcode/).

To build AlphaLive Audio Library Installer on Windows you will need to download and install [Visual Studio](https://visualstudio.microsoft.com/downloads/) with the C++ SDK.

## Building ##

For all platforms, use the 'Audio Library Installer.jucer' Projucer file to configure the project for your system.

On Windows, the application must be built to [run using administrator permissions](https://docs.microsoft.com/en-us/cpp/build/reference/manifestuac-embeds-uac-information-in-manifest?view=msvc-160) in order for the application to work correctly.

For this application to correctly work the files that need to be installed must be in the following file hierarchy from the application file:

+  Audio Library - "../Data/Audio Library/"
+  Demo Project - "../Data/Demo Project/"
+  Tutorial Project - "../Data/Tutorial Project/"

## License ##

AlphaLive Audio Library Installer is licensed under the GPLv3. See gpl-3.0.txt for details.

AlphaLive and the AlphaSphere have been developed by [nu desine](https://www.watershed.co.uk/studio/residents/nu-desine).

The following libraries are included in the source code:

+  The [JUCE](https://juce.com) library which is licensed under the GPLv3. 


