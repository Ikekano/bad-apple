#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

void processFrame(const cv::Mat& frame, cv::Mat& outputFrame, int blockSize, const cv::Mat& blackImg, const cv::Mat& whiteImg) {
    int rows = frame.rows / blockSize;
    int cols = frame.cols / blockSize;
    
    cv::Mat resized;
    cv::resize(frame, resized, cv::Size(cols, rows));
    cv::cvtColor(resized, resized, cv::COLOR_BGR2GRAY);
    
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

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cout << "Usage: " << argv[0] << " <video_file> <block_size> <black_pixel_image> <white_pixel_image> <output_video>" << std::endl;
        return -1;
    }
    
    std::string videoPath = argv[1];
    int blockSize = std::stoi(argv[2]);
    std::string blackPixelImage = argv[3];
    std::string whitePixelImage = argv[4];
    std::string outputVideo = argv[5];
    
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file." << std::endl;
        return -1;
    }
    
    cv::Mat blackImg = cv::imread(blackPixelImage);
    cv::Mat whiteImg = cv::imread(whitePixelImage);
    
    if (blackImg.empty() || whiteImg.empty()) {
        std::cerr << "Error loading replacement images." << std::endl;
        return -1;
    }
    
    cv::Mat frame, outputFrame;
    
    // Use avc1 codec and output as MP4
    int fourcc = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
    cv::Size frameSize(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    cv::VideoWriter videoWriter(outputVideo, fourcc, fps, frameSize);
    
    if (!videoWriter.isOpened()) {
        std::cerr << "Error: Could not open the output video file for writing." << std::endl;
        return -1;
    }
    
    cv::setNumThreads(4); // Enable multi-threading for performance boost
    
    while (cap.read(frame)) {
        processFrame(frame, outputFrame, blockSize, blackImg, whiteImg);
        videoWriter.write(outputFrame);
        //cv::imshow("Output", outputFrame);
        //if (cv::waitKey(33) == 27) break; // Press ESC to exit
    }
    
    return 0;
}
