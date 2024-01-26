# UFO Detector 2.0

The UFO Detector is a software that allows the autonomous video recording of unidentifiable flying objects. This
includes [unknown aircrafts](http://ufoid.net/videos/2-objects-flying-close-together), [meteors](http://ufoid.net/videos/meteor-captured-in-north-hollywood-ca)
or other [aerial anomalies](http://ufoid.net/videos/object-disappearing-in-clouds). It is designed to detect and record
objects that cannot be identified as something known during the day or night. [See it in action]

This 2.0 version was designed to work on an embedded device such as Raspberry Pi. It is a fork of
the [original project](https://github.com/UFOID/UFO-Detector).

### Features:

- Autonomous video recording of possible UFO’s
- Interface to manage all the recorded videos
- Selection of specific detection area

### Tech Stack:

- C++
- OpenCV
- Qt

### ToDo:

This is an early prototype. It "works" but there are still many things to do:

- Find and document supported hardware (camera, board, case, etc.)
- Cleanup code. Add documentation, tests, etc.
- Add support for more cameras, boards
- Improve detection algorithm

Contributions are welcome!

### Building and running:

- See [raspberrypi.md](raspberrypi.md) for Raspberry Pi
- See [firefly.md](firefly.md) for Firefly

### Detection Algorithm:

Since the objects that the software looks for are not well defined (i.e. being “unknown”) false positive detections are
expected. We use a combination of following algorithms in order to narrow down the results:

1. Motion Detection: The program looks for moving objects within the selected area of detection. The user can specify
   the size of noise which will be ignored. Each moving object is tracked.
2. Light detection: We assume any UFO is light emitting (i.e. object being brighter than the background) in order to
   decrease the amount of false-positives.
3. Object recognition: Each detected bright object is analyzed using cascade classifier which was trained using images
   of flying birds. This is only used if the detection process is run during the day. Currently only birds are filtered
   out in this way.
4. Aircraft filter: Using data from the internet to check whether there are aircrafts within the camera view. (Not
   implemented in this version)

### License:

GPLv3

### Credits

https://github.com/Smorodov/Multitarget-tracker/

[See it in action]: https://www.youtube.com/watch?v=Jai15P4kuug
