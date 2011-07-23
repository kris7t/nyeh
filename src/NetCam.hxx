#pragma once

#include "Cam.hxx"
#include "Net.hxx"

class NetCam : public Cam {
public:
    NetCam();
    NetCam(const std::string & host);

    void grabImage();
    void push(const cv::Mat& jpeg);

    const cv::Mat & jpeg() const;
    const cv::Mat & frame() const;

private:
    cv::Mat jpeg_, frame_;

    NetConnection conn;
};
