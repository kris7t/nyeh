#pragma once

class Cam;
typedef std::tr1::shared_ptr<Cam> Cam_;

class Cam {
public:
    virtual ~Cam() {}

    virtual void grabImage() = 0;
    virtual const cv::Mat& jpeg() const = 0;
    virtual const cv::Mat& frame() const = 0;
    
    cv::Size size() const {
        return size_;
    }

    static Cam_ create(cv::Size size, int camId = 0, bool jpg = false);
protected:
    Cam(cv::Size size);
private:
    const cv::Size size_;
};
