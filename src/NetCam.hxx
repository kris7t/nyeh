#pragma once

#include "Cam.hxx"
#include "Net.hxx"

class NetClientCam : public Cam {
public:
    NetClientCam(const std::string & host);

    void grabImage();

    const cv::Mat & jpeg() const;
    const cv::Mat & frame() const;

private:
    cv::Mat jpeg_, frame_;

    NetSocket sock;
};

class NetServerCam {
public:
    NetServerCam();

    void push(const cv::Mat& jpeg);

private:
    NetServer srv;
    NetSocket_ sock;
};
