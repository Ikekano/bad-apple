# Bad Apple PNG Grid Generator
This project requires installing OpenCV and ffmpeg. Can be used in a windows environment but linux is preferred. I use WSL and it works just fine.

## Setup and Installation

### 1. Setting up OpenCV

    sudo apt install libopencv-dev python3-opencv

### 2. Setting up ffmpeg

    sudo apt install libx264-dev ffmpeg

### 3. Setting up the project

    g++ badapple.cpp -o badapple -I/usr/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs

### 4. Running the project

    ./badapple badapple.mp4 60
    
    ffmpeg -i output.mp4 -i badapple.mp3 -c:v copy -c:a aac -b:a 116k -map 0:v:0 -map 1:a:0 -shortest -y output-audio.mp4
