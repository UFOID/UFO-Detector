# UFO Detector

The UFO Detector is a software that allows the autonomous video recording of possible UFOs. It is designed to detect and record objects that cannot be identified as something know during the day or night. [See it in action]

### Features:
  - Autonomous video recording of possible UFO’s
  - Interface to manage all the recorded videos
  - Sharing of captured video with UFOID.net
  - Selection of specific detection area
  - (Planned) Cross-platform support
  
### Requirements:
  - USB Webcam
  - OpenCV 3.0
  - Qt 5.6.0
  - FFmpeg / avconv
  
### ToDo:
  - Interface to support more video devices
  - Cross-platform support
  - Improve detection algorithm
  - Many bug fixes

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
