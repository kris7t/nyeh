#pragma once

class Cam;
typedef std::tr1::shared_ptr<Cam> Cam_;

class Cam {
public:
    virtual ~Cam() {}

    virtual void grabImage() = 0;
    virtual const cv::Mat& jpeg() const = 0;
    virtual const cv::Mat& frame() const = 0;

    static Cam_ create(int camId);
};
