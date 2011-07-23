#pragma once

#include "Cam.hxx"

class Hand {
    public:
        virtual void calibrate(Cam_ cam) =0;
        virtual void update(const cv::Mat & frame) =0;
        virtual const cv::Point3f & position() const =0;
        virtual const cv::Point3f & velocity() const =0;
};
