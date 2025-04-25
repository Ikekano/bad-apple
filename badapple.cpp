#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

void processFrame(const cv::Mat& frame, int blockSize, bool textMode, const cv::Mat& blackImg, const cv::Mat& whiteImg, cv::Mat& outputFrame) {
    int rows = frame.rows / blockSize;
    int cols = frame.cols / blockSize;

    cv::Mat resized;
    cv::resize(frame, resized, cv::Size(cols, rows));
    cv::cvtColor(resized, resized, cv::COLOR_BGR2GRAY);

    if (textMode) {
        std::cout << "\033[H"; // Move cursor to home position
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                uchar pixel = resized.at<uchar>(i, j);
                std::cout << (pixel < 128 ? '0' : '1') << (pixel < 128 ? '0' : '1');
            }
            std::cout << '\n';
        }
        std::cout.flush();
    } else {
        outputFrame = cv::Mat::zeros(frame.size(), frame.type());
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                uchar pixel = resized.at<uchar>(i, j);
                cv::Mat roi = outputFrame(cv::Rect(j * blockSize, i * blockSize, blockSize, blockSize));
                if (pixel < 128) {
                    blackImg.copyTo(roi);
                } else {
                    whiteImg.copyTo(roi);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cout << "Usage: " << argv[0] << " <video_file> <block_size> <black_pixel_image> <white_pixel_image> <output_video> [--text]" << std::endl;
        return -1;
    }

    std::string videoPath = argv[1];
    int blockSize = std::stoi(argv[2]);
    std::string blackPixelImage = argv[3];
    std::string whitePixelImage = argv[4];
    std::string outputVideo = argv[5];
    bool textMode = (argc >= 7 && std::string(argv[6]) == "--text");

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file." << std::endl;
        return -1;
    }

    cv::Mat blackImg = cv::imread(blackPixelImage);
    cv::Mat whiteImg = cv::imread(whitePixelImage);

    if (!textMode && (blackImg.empty() || whiteImg.empty())) {
        std::cerr << "Error loading replacement images." << std::endl;
        return -1;
    }

    cv::Mat frame, outputFrame;

    cv::VideoWriter videoWriter;
    if (!textMode) {
        int fourcc = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
        cv::Size frameSize(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        double fps = cap.get(cv::CAP_PROP_FPS);
        videoWriter.open(outputVideo, fourcc, fps, frameSize);

        if (!videoWriter.isOpened()) {
            std::cerr << "Error: Could not open the output video file for writing." << std::endl;
            return -1;
        }
    }

    cv::setNumThreads(4);
    int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
    double fps = cap.get(cv::CAP_PROP_FPS);
    int currentFrame = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    if (textMode) std::cout << "\033[2J"; // Clear screen once

    while (cap.read(frame)) {
        processFrame(frame, blockSize, textMode, blackImg, whiteImg, outputFrame);

        if (!textMode) {
            videoWriter.write(outputFrame);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0 / fps)));
        }

        currentFrame++;
        if (!textMode)
            std::cout << "\rProgress: Frame " << currentFrame << " / " << totalFrames << std::flush;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    std::cout << std::endl;
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Processing completed in " << elapsed_time.count() << " seconds." << std::endl;

    double avgfps = totalFrames / elapsed_time.count();
    double timeframe = elapsed_time.count() / totalFrames;
    std::cout << "Average Time per Frame: " << timeframe << " seconds. (" << avgfps << " fps)" << std::endl;

    return 0;
}
