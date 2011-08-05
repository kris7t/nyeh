#pragma once

#include "Cam.hxx"

class HighguiCam : public Cam {
public:
    HighguiCam(cv::Size size, int device = 0);

    void grabImage();

    const cv::Mat& jpeg() const;
    const cv::Mat& frame() const;

private:
    cv::Mat jpeg_;
    cv::Mat frame_;

    cv::VideoCapture cap_;
};
