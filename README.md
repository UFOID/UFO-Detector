# UFO Detector

The UFO Detector is a software that allows the autonomous video recording of possible UFOs. It is designed to detect and record objects that cannot be identified as something known during the day or night. [See it in action]

### Features:
  - Autonomous video recording of possible UFO’s
  - Interface to manage all the recorded videos
  - Sharing of captured video with UFOID.net
  - Selection of specific detection area
  - Currently support in Windows and Linux. Raspberry Pi has been planned (https://github.com/UFOID/UFO-Detecor-Pi).
  
### Requirements:
  - USB Webcam
  - OpenCV 3.0 (2.4.8 will work in Ubuntu 14, but 3.1.0 is required in Ubuntu >=15)
  - Qt 5
  - FFmpeg / avconv
  
### ToDo:
  - Interface to support more video devices
  - Improve detection algorithm
  - Many bug fixes

### Building and Running

First, get the source code for UFO Detector by either cloning this repository or downloading the source as a ZIP package. See the "Clone or download" button above.

#### Windows

1. Install the prerequisites, see the list above.
2. Open the project file ...\UFO-Detector\ui-qt\Detector.pro in Qt Creator. Edit OpenCV paths in opencv.pri so they point to correct locations.
3. Build the project. You can run the application from Qt Creator or from command line in the build directory.

#### Generic Notes for Building in Linux

If your distribution has OpenCV version other than 2.4.9 through 3.0.0, you *should* be able to build UFO Detector with the development packages from the normal repositories. In that case, read the instructions for Ubuntu Linux 14 and apply them to your distribution. If UFO Detector has problem writing videos correctly you know you have the buggy version of OpenCV, in which case read the next paragraph.

OpenCV versions between 2.4.9 through 3.0.0 have a [bug](https://github.com/opencv/opencv/issues/5439), at least in Ubuntu Linux, causing video writing to fail. This means OpenCV >=3.1.0 need to be installed by hand. For that, read the instructions for Ubuntu Linux >=15 and apply them to your distribution.

#### Ubuntu Linux 14

First, install Qt5, avconv/ffmpeg, and OpenCV development packages:

```sudo apt-get install qtbase5-dev avconv libopencv-dev```

If this is not enough, check if you need to install other development packages as well.

Open the project file .../UFO-Detector/ui-qt/Detector.pro in Qt Creator and build it. You can run the application from Qt Creator or from command line in the build directory.

#### Ubuntu Linux >=15

In Ubuntu Linux >=15, first install Qt5 and avconv/ffmpeg, plus the necessary packages for compiling OpenCV. Installing libopencv-dev package is optional because it will not be used.

```sudo apt-get install qtbase5-dev avconv build-essential cmake libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev```

Next, download [OpenCV 3.1.0 source package](http://opencv.org/downloads.html) for Linux and unzip it. It's assumed here the target folder is opencv-3.1.0. Then run the following commands in the same folder in which you unzipped the source package:

```
mkdir opencv-3.1.0-build
cd opencv-3.1.0-build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D WITH_IPP=OFF -D CMAKE_INSTALL_PREFIX=/usr/local ../opencv-3.1.0
make
sudo make install
sudo ldconfig
```

If you use IPP ICV, turn it ON and figure how to make it work (didn't work out of the box).

For more info about OpenCV, please refer to [OpenCV building instructions](http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html).

Open the project file .../UFO-Detector/ui-qt/Detector.pro in Qt Creator and build it. You can run the application from Qt Creator or from command line in the build directory.

### Detection Algorithm:
Since the objects that the software looks for are not well defined (i.e. being “unknown”) false positive detections are expected.  We use a combination of following algorithms in order to narrow down the results:

1.	Motion Detection: The program looks for moving objects within the selected area of detection. The user can specify the size of noise which will be ignored. Each moving object is tracked.
2.	Light detection: We assume any UFO is light emitting (i.e. object being brighter than the background) in order to decrease the amount of false-positives.
3.	Object recognition: Each detected bright object is analyzed using cascade classifier which was trained using images of flying birds. This is only used if the detection process is run during the day. Currently only birds are filtered out in this way.

### License:
GPLv3

### Credits
https://github.com/Smorodov/Multitarget-tracker/

   [See it in action]: https://www.youtube.com/watch?v=Jai15P4kuug
