# Hardware

- [Raspberry Pi 4 Model B](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/)
- [IMX462](https://www.veye.cc/en/product/veye-mipi-imx462/)

# Build
sudo apt update
sudo apt upgrade

## Ffmpeg
`sudo apt install pkg-config     
sudo apt install ffmpeg libavformat-dev libavcodec-dev libswscale-dev
`
## Gstreamer
`sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-alsa gstreamer1.0-pulseaudio
`

## Opencv

- https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

## Qt

- https://wiki.qt.io/Building_Qt_6_from_Git

`perl init-repository --module-subset=qthttpserver,qtwebsockets`
`../qt6/configure -no-gui`

## IMX462

- https://wiki.veye.cc/index.php/V4L2_mode_for_Raspberry_Pi
- https://wiki.veye.cc/index.php/CS-MIPI-X_for_Raspberry_Pi