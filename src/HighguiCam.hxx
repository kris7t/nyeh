#pragma once

class HighguiCam {
public:
    HighguiCam(int device = 0);

    void grabImage();

    const cv::Mat& jpeg() const {
        return jpeg_;
    }
    const cv::Mat& frame() const {
        return frame_;
    }

private:
    cv::Mat jpeg_;
    cv::Mat frame_;

    cv::VideoCapture cap_;
};
