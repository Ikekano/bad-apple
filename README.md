# Bad Apple PNG Grid Generator
This project comes from the internet [meme](https://knowyourmeme.com/memes/bad-apple) and [rule](https://tropedia.fandom.com/wiki/Rules_of_the_Internet) of turning everything into [Bad Apple](https://www.youtube.com/watch?v=FtutLA63Cp8).

This project requires installing OpenCV and ffmpeg. Can be used in a windows environment but linux is preferred. I use WSL with Ubuntu and it works just fine.

Since this project is in C++, **g++/gcc** will also be necessary to install if not done already.

Python is also supported now, it is about **62-82.7%** slower and seems to produce a file size that's about **68.5-131.7%** larger but hey, it's Python

## Setup and Installation (C++)

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
>
> The repo **INCLUDES** 2 example PNG files that are 20x20 pixels. These can be used to test if the program is working as intended.

3. Compile the program by running the following g++ compiling command.

        g++ badapple.cpp -o badapple -I/usr/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs

### 4. Running the project

1. Once sucessfully compiled, run the program by using the command below:

> [!Note]
> **Command format:** ./badapple \<input filename> \<pixel size (W/H)> \<black pixel filename> \<white pixel filename> \<output filename>
>
> The following example line uses the included example PNGs called black.png and white.png with a size of 20x20

        ./badapple badapple.mp4 20 black.png white.png output.mp4

2. Run the following ffmpeg command if you want to remux the audio to the output video from the program as it will not retain the audio from the input video.
    
        ffmpeg -i output.mp4 -i badapple.mp3 -c:v copy -c:a aac -b:a 116k -map 0:v:0 -map 1:a:0 -shortest -y output-audio.mp4

> [!Note]
> This project will work with higher resolution and higher framerate versions of bad apple. Those versions will **NOT** be explicitly provided in this repo, but can be integrated into the project very easily.
>
> Processing time will vary wildly depending on the increased resolution and framerate of the input video.

## Setup and Installation (Python)

> [!Note]
> **The following commands are for an Ubuntu/Debian environment. If you are using a different distro then you need to look up the correct command.**

Before doing the commands below make sure that you update apt using the command below:

    sudo apt update

### 1. Setting up [OpenCV](https://opencv.org/)

Install the OpenCV libraries using the command below:

    pip install opencv-python

### 2. Setting up [PyAV](https://github.com/PyAV-Org/PyAV)

Install the PyAV libraries using the command below:

    pip install av
    
### 3. Setting up ffmpeg (for audio)

Install the ffmpeg libraries using the command below:

    sudo apt install libx264-dev ffmpeg

### 4. Running the project

1. Once sucessfully setup, run the program by using the command below:

> [!Note]
> **Command format:** python3 badapple.py \<input filename> \<pixel size (W/H)> \<black pixel filename> \<white pixel filename> \<output filename>
>
> The following example line uses the included example PNGs called black.png and white.png with a size of 20x20

        python3 badapple.py badapple.mp4 20 black.png white.png output.mp4

2. Run the following ffmpeg command if you want to remux the audio to the output video from the program as it will not retain the audio from the input video.
    
        ffmpeg -i output.mp4 -i badapple.mp3 -c:v copy -c:a aac -b:a 116k -map 0:v:0 -map 1:a:0 -shortest -y output-audio.mp4

> [!Note]
> This project will work with higher resolution and higher framerate versions of bad apple. Those versions will **NOT** be explicitly provided in this repo, but can be integrated into the project very easily.
>
> Processing time will vary wildly depending on the increased resolution and framerate of the input video.





