#pragma once

#include "Cam.hxx"

class Hand {
    public:
        virtual void calibrate(Cam_ cam) =0;
        virtual void update(const cv::Mat & frame) =0;
        virtual cv::Point3f position() volatile const =0;
        virtual cv::Point3f velocity() volatile const =0;
        virtual double minRadius() const =0;
        virtual double maxRadius() const =0;
        virtual double kappa() const =0;
};

typedef std::tr1::shared_ptr<Hand> Hand_;
