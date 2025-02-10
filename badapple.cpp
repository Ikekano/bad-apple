#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>

//#include <opencv2/cudaimgproc.hpp>
//#include <opencv2/cudawarping.hpp>
#include <opencv2/core/ocl.hpp>

// CPU processing
void processFrameCPU(const cv::Mat& frame, cv::Mat& outputFrame, int blockSize, const cv::Mat& blackImg, const cv::Mat& whiteImg) {
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

// Open CL processing
void processFrameOpenCL(const cv::UMat& frame, cv::UMat& outputFrame, int blockSize, const cv::UMat& blackImg, const cv::UMat& whiteImg) {
    int rows = frame.rows / blockSize;
    int cols = frame.cols / blockSize;
    
    cv::UMat resized, gray;
    cv::resize(frame, resized, cv::Size(cols, rows));
    cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
    
    outputFrame.create(frame.size(), frame.type());
    outputFrame.setTo(cv::Scalar(0, 0, 0));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            uchar pixel = gray.getMat(cv::ACCESS_READ).at<uchar>(i, j);
            cv::Rect roiRect(j * blockSize, i * blockSize, blockSize, blockSize);
            if (pixel < 128) {
                blackImg.copyTo(outputFrame(roiRect));
            } else {
                whiteImg.copyTo(outputFrame(roiRect));
            }
        }
    }
}

// CUDA processing
/*
void processFrameGPU(const cv::cuda::GpuMat& frame, cv::cuda::GpuMat& outputFrame, int blockSize, const cv::cuda::GpuMat& blackImg, const cv::cuda::GpuMat& whiteImg) {
    int rows = frame.rows / blockSize;
    int cols = frame.cols / blockSize;
    
    cv::cuda::GpuMat resized, gray;
    cv::cuda::resize(frame, resized, cv::Size(cols, rows));
    cv::cuda::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
    
    outputFrame.create(frame.size(), frame.type());
    outputFrame.setTo(cv::Scalar(0, 0, 0));
    
    cv::Mat hostGray;
    gray.download(hostGray);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            uchar pixel = hostGray.at<uchar>(i, j);
            cv::Rect roiRect(j * blockSize, i * blockSize, blockSize, blockSize);
            if (pixel < 128) {
                blackImg.copyTo(outputFrame(roiRect));
            } else {
                whiteImg.copyTo(outputFrame(roiRect));
            }
        }
    }
}
*/

int main(int argc, char** argv) {
 
    if (argc < 7) 
    {
        //std::cout << "Usage: " << argv[0] << " <video_file> <block_size> <black_pixel_image> <white_pixel_image> <output_video> <mode: 0=CPU, 1=OpenCL, 2=CUDA>" << std::endl;
	std::cout << "Usage: " << argv[0] << " <video_file> <block_size> <black_pixel_image> <white_pixel_image> <output_video> <mode: 0=CPU, 1=OpenCL>" << std::endl;
	
        return -1;
    }
    
    std::string videoPath = argv[1];
    int blockSize = std::stoi(argv[2]);
    std::string blackPixelImage = argv[3];
    std::string whitePixelImage = argv[4];
    std::string outputVideo = argv[5];
    int mode = std::stoi(argv[6]);
    
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file." << std::endl;
        return -1;
    }
    
    cv::Mat blackImgCPU = cv::imread(blackPixelImage);
    cv::Mat whiteImgCPU = cv::imread(whitePixelImage);
    
    //cv::cuda::GpuMat blackImgGPU, whiteImgGPU;
    cv::UMat blackImgOCL, whiteImgOCL;
    
    if (mode == 1) {
        cv::ocl::setUseOpenCL(true);
        blackImgCPU.copyTo(blackImgOCL);
        whiteImgCPU.copyTo(whiteImgOCL);
    } else if (mode == 0) {
        cv::setNumThreads(4); // Enable multi-threading for performance boost
    }
    
    /*
    else if (mode == 2) {
        blackImgGPU.upload(blackImgCPU);
        whiteImgGPU.upload(whiteImgCPU);
    }
    */
    
    if (blackImgCPU.empty() || whiteImgCPU.empty()) {
        std::cerr << "Error loading replacement images." << std::endl;
        return -1;
    }
    
    cv::Mat frameCPU, outputFrameCPU;
    cv::UMat frameOCL, outputFrameOCL;
    //cv::cuda::GpuMat frameGPU, outputFrameGPU;
    
    // Use avc1 codec and output as MP4
    int fourcc = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
    cv::Size frameSize(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    cv::VideoWriter videoWriter(outputVideo, fourcc, fps, frameSize);
    
    if (!videoWriter.isOpened()) {
        std::cerr << "Error: Could not open the output video file for writing." << std::endl;
        return -1;
    }
  
    int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int currentFrame = 0;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    while (cap.read(frameCPU)) {
    
	if (mode == 1) {
            frameCPU.copyTo(frameOCL);
            processFrameOpenCL(frameOCL, outputFrameOCL, blockSize, blackImgOCL, whiteImgOCL);
            outputFrameOCL.copyTo(outputFrameCPU);
        } else if (mode == 0) {
            processFrameCPU(frameCPU, outputFrameCPU, blockSize, blackImgCPU, whiteImgCPU);
        }
	
	/*
	else if (mode == 2) {
            frameGPU.upload(frameCPU);
            processFrameGPU(frameGPU, outputFrameGPU, blockSize, blackImgGPU, whiteImgGPU);
            outputFrameGPU.download(outputFrameCPU);
        } 
	*/
	
        //cv::imshow("Output", outputFrame);
        //if (cv::waitKey(33) == 27) break; // Press ESC to exit
	videoWriter.write(outputFrameCPU);
	currentFrame++;
        std::cout << "\rProgress: Frame " << currentFrame << " / " << totalFrames << std::flush;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    
    std::cout << std::endl;
    
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Processing completed in " << elapsed_time.count() << " seconds." << std::endl;
    
    double avgfps = totalFrames/elapsed_time.count();
    double timeframe = elapsed_time.count()/totalFrames;
    std::cout << "Average Time per Frame: " << timeframe << " seconds. (" << avgfps << " fps)" << std::endl;
    
    return 0;
}
