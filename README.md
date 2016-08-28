# UFO Detector

The UFO Detector is a software that allows the autonomous video recording of possible UFOs. It is designed to detect and record objects that cannot be identified as something know during the day or night. [See it in action]

### Features:
  - Autonomous video recording of possible UFO’s
  - Interface to manage all the recorded videos
  - Sharing of captured video with UFOID.net
  - Selection of specific detection area
  - Currently support in Windows and Linux. Raspberry Pi has been planned (https://github.com/UFOID/UFO-Detecor-Pi).
  
### Requirements:
  - USB Webcam
  - OpenCV 3.0 (2.4 will work, too)
  - Qt 5
  - FFmpeg / avconv
  
### ToDo:
  - Interface to support more video devices
  - Improve detection algorithm
  - Many bug fixes

### Building and Running
1. Install the prerequisites, see the list above.

  **Ubuntu Linux**

  In a single command in Ubuntu Linux (at least 16.04, other versions shouldn't differ too much): ```sudo apt-get install qtbase5-dev libopencv-core-dev libopencv-calib3d-dev libopencv-features2d-dev libopencv-flann-dev libopencv-highgui-dev libopencv-imgproc-dev libopencv-ml-dev libopencv-objdetect-dev libopencv-video-dev avconv```
2. Open the Qt project file, currently UFO-Detector/src/Detector.pro, in Qt Creator.
3. First build the project and then run either in Qt Creator or in command line in build target folder.

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
