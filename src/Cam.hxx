#pragma once

class Cam {
public:
    virtual ~Cam() {}

    virtual void grabImage() = 0;
    virtual const cv::Mat& jpeg() const = 0;
    virtual const cv::Mat& frame() const = 0;

    static std::tr1::shared_ptr<Cam> create(int camId);
};
