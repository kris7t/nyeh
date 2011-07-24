#pragma once

#include "Cam.hxx"

class Hand {
    public:
        virtual void calibrate(Cam_ cam) =0;
        virtual void update(const cv::Mat & frame) =0;
        virtual volatile const cv::Point3f & position() volatile const =0;
        virtual volatile const cv::Point3f & velocity() volatile const =0;
        virtual volatile const double & minRadius() volatile const =0;
        virtual volatile const double & maxRadius() volatile const =0;

};

typedef std::tr1::shared_ptr<Hand> Hand_;
