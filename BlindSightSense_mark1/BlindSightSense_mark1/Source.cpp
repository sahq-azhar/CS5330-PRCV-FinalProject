#include <opencv2/opencv.hpp>
#include <iostream>
#include <Windows.h> // Required for playing sound on Windows platform

using namespace cv;

int main()
{
    // Create a VideoCapture object to capture live video input
    VideoCapture cap(0);

    // Check if camera is opened successfully
    if (!cap.isOpened()) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    // Load the sound file
    const std::string sound_file = "stop.wav";
    std::wstring wide_sound_file(sound_file.begin(), sound_file.end()); // Convert to wide string for Windows API
    PlaySound(wide_sound_file.c_str(), NULL, SND_FILENAME | SND_ASYNC);

    while (1) {

        // Read a frame from the video input
        Mat frame;
        cap.read(frame);

        // Convert the frame from BGR to HSV color space
        Mat hsv_frame;
        cvtColor(frame, hsv_frame, COLOR_BGR2HSV);

        // Apply color thresholding to extract the red regions
        Mat red_mask;
        inRange(hsv_frame, Scalar(0, 70, 50), Scalar(10, 255, 255), red_mask);
        inRange(hsv_frame, Scalar(170, 70, 50), Scalar(180, 255, 255), red_mask);

        // Apply morphological operations to remove noise and fill gaps
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
        morphologyEx(red_mask, red_mask, MORPH_OPEN, kernel);
        morphologyEx(red_mask, red_mask, MORPH_CLOSE, kernel);

        // Find contours in the binary image
        std::vector<std::vector<Point>> contours;
        findContours(red_mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // Loop over all the contours
        for (size_t i = 0; i < contours.size(); i++) {

            // Compute the contour area
            double area = contourArea(contours[i]);

            // If the area is within a certain range, draw a bounding box around it and print a message
            if (area > 1000 && area < 10000) {
                Rect rect = boundingRect(contours[i]);
                rectangle(frame, rect, Scalar(0, 0, 255), 2);
                std::cout << "Stop sign detected!" << std::endl;

                // Play the sound file
                PlaySound(wide_sound_file.c_str(), NULL, SND_FILENAME | SND_ASYNC);
            }
            else
                std::cout << "Stop sign not detected!" << std::endl;
        }

        // Display the video feed with the identified stop sign
        imshow("Stop Sign Detection", frame);

        // Press ESC key to exit the loop
        char c = (char)waitKey(25);
        if (c == 27) break;
    }

    // Release the VideoCapture object
    cap.release();

    // Close all the windows
    destroyAllWindows();

    return 0;
}
