#pragma once

#include "Cam.hxx"

class UvcCam : public Cam {
public:
    UvcCam(int device = 0);
    ~UvcCam();

    void grabImage();

    const cv::Mat& jpeg() const;
    const cv::Mat& frame() const;

private:
    void destroy();
    void disable(const std::string& name, int id);

    cv::Mat jpeg_;
    cv::Mat frame_;

    static const size_t BUFFERS = 10;

    int fd;
    void * map[BUFFERS];
    size_t len;
};
