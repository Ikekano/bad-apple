# Use a base image for armv7 with build tools
FROM arm32v7/debian:bullseye

# Set environment to noninteractive to avoid prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages and SDL2 dependencies
RUN     apt-get update && \
        apt-get install -y \
        build-essential \
        git \
        libsdl2-dev \
        libsdl2-image-dev \
        libsdl2-mixer-dev \
        libsdl2-ttf-dev \
        libopencv-dev \
        python3-opencv \
        libx264-dev \
        ffmpeg \
        && rm -rf /var/lib/apt/lists/*

# Create a working directory
WORKDIR /app

# Clone the bad-apple repo
RUN     git clone --progress --verbose https://github.com/Ikekano/bad-apple.git

# Change directory to build
WORKDIR /app/bad-apple

# Compile the program
RUN     g++ badapple.cpp -o badapple -I/usr/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs

# Command to run the program
#CMD     ["./badapple", "badapple.mp4", "20", "black.png", "white.png", "output.mp4"]
CMD     ["./badapple", "badapple.mp4", "20", "--text"]
