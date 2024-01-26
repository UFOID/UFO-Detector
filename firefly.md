> work-in-progress. The firefly version does not work properly and only achieves 15 FPS.

# Hardware

- [ROC-RK3588S-PC](https://wiki.t-firefly.com/en/ROC-RK3588-PC/overview.html)
- [IMX462](https://www.veye.cc/en/product/veye-mipi-imx462/)

## Install image ROC-RK3588S-PC_Ubuntu_v1.0.4b_221226_veyecam2m

### test camera
dmesg | grep veye
gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=10 ! video/x-raw,format=NV12,width=1920,height=1080 ! mppjpegenc ! multifilesink location=./test%05d.jpg
gst-launch-1.0 v4l2src device=/dev/video0 num-buffers=100 !  video/x-raw,format=NV16,width=1920,height=1080,framerate=30/1 ! queue! videoconvert ! mpph264enc ! queue ! h264parse ! mp4mux !  filesink location=./h264.mp4


sudo apt update

## gstreamer

sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

## opencv

sudo apt install cmake g++ g++-10 wget unzip

wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.x.zip
unzip opencv.zip
unzip opencv_contrib.zip
### Create build directory and switch into it
mkdir -p build && cd build
### Configure
cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.x/modules ../opencv-4.x
### Build
cmake --build . -j8
sudo make install