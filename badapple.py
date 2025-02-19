import cv2
import numpy as np
import sys
import time

import av

# pip install opencv-python
# pip install av

def process_frame(frame, block_size, black_img, white_img):
    rows, cols = frame.shape[0] // block_size, frame.shape[1] // block_size

    resized = cv2.resize(frame, (cols, rows))
    gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)

    output_frame = np.zeros(frame.shape, dtype=frame.dtype)

    for i in range(rows):
        for j in range(cols):
            pixel = gray[i, j]
            roi = output_frame[i * block_size:(i + 1) * block_size, j * block_size:(j + 1) * block_size]
            if pixel < 128:
                roi[:] = black_img
            else:
                roi[:] = white_img
    return output_frame

def main():
    if len(sys.argv) < 6:
        print("Usage: python3 badapple.py <video_file> <block_size> <black_pixel_image> <white_pixel_image> <output_video>")
        sys.exit(-1)

    video_path = sys.argv[1]
    block_size = int(sys.argv[2])
    black_pixel_image = sys.argv[3]
    white_pixel_image = sys.argv[4]
    output_video = sys.argv[5]

    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        print("Error: Could not open video file.")
        sys.exit(-1)

    black_img = cv2.imread(black_pixel_image)
    white_img = cv2.imread(white_pixel_image)

    if black_img is None or white_img is None:
        print("Error: Could not load replacement images.")
        sys.exit(-1)

    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    
    # Python version of openCV does not support native H.264 encoding
    # PyAV is better and will be used instead
    output_container = av.open(output_video, mode='w')
    stream = output_container.add_stream('libx264', rate=int(fps))
    stream.width = frame_width
    stream.height = frame_height
    stream.pix_fmt = 'yuv420p'  # Common pixel format for H.264 encoding
    
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    current_frame = 0

    start_time = time.time()

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        output_frame = process_frame(frame, block_size, black_img, white_img)
        #out.write(output_frame)
        
        # Convert frame to PyAV format
        frame_av = av.VideoFrame.from_ndarray(output_frame, format='bgr24')
        packet = stream.encode(frame_av)
        if packet:
            output_container.mux(packet)

        current_frame += 1
        print(f"\rProgress: Frame {current_frame} / {total_frames}", end="")
        
    # Flush the encoder
    packet = stream.encode(None)
    if packet:
        output_container.mux(packet)

    output_container.close()

    end_time = time.time()

    elapsed_time = end_time - start_time
    avg_fps = total_frames / elapsed_time
    time_per_frame = elapsed_time / total_frames
    print(f"\nProcessing completed in {elapsed_time:.5f} seconds.")
    print(f"Average Time per Frame: {time_per_frame:.9f} seconds. ({avg_fps:.2f} fps)")

    cap.release()
    #out.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()