# Bad Apple PNG Grid Generator
This project requires installing OpenCV and ffmpeg. Can be used in a windows environment but linux is preferred. I use WSL and it works just fine.

Since this project is in C++, **g++/gcc** will also be necessary to install if not done already.

## Setup and Installation

> [!Note]
> **The following commands are for an Ubuntu/Debian environment. If you are using a different distro then you need to look up the correct command.**

Before doing the commands below make sure that you update apt using the command below:

    sudo apt update

### 1. Setting up [OpenCV](https://opencv.org/)

Install the OpenCV libraries using the command below:

    sudo apt install libopencv-dev python3-opencv

### 2. Setting up ffmpeg (for audio)

Install the ffmpeg libraries using the command below:

    sudo apt install libx264-dev ffmpeg

### 3. Setting up the project

1. To compile the project, navigate to the folder that this repo is in using the appropriate cd commands.
2. Once in the working directory with the badapple.cpp file prepare the 2 PNG image files that are going to be used in the video.

> [!Note]
> **The images that are used NEED TO BE SQUARE!! and please take note of the width/height of the image you are using!**

3. Compile the program by running the following g++ compiling command.

        g++ badapple.cpp -o badapple -I/usr/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs

### 4. Running the project

1. Once sucessfully compiled run the program by using the command below:

        ./badapple badapple.mp4 20 black.png white.png output.mp4

2. Run the following ffmpeg command if you want to remux the audio to the output video from the program as it will not retain the audio from the input video.
    
        ffmpeg -i output.mp4 -i badapple.mp3 -c:v copy -c:a aac -b:a 116k -map 0:v:0 -map 1:a:0 -shortest -y output-audio.mp4
