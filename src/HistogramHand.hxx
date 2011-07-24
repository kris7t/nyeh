#pragma once

#include "Hand.hxx"

class HistogramHand : public Hand {
    public:
        HistogramHand(double fillratio, double t);
        virtual void calibrate(Cam_ cam);
        virtual void update(const cv::Mat & frame);
        virtual volatile const cv::Point3f & position() volatile const;
        virtual volatile const cv::Point3f & velocity() volatile const;
        virtual volatile const double & minRadius() volatile const;
        virtual volatile const double & maxRadius() volatile const;

    private:
        void measureHist(Cam_ cap, cv::MatND & hist, cv::Point center, int radius, bool accumulate = true);
        double measureRadius(Cam_ cap);

        volatile cv::Point3f position_;
        volatile cv::Point3f velocity_;
        volatile double minRadius_;
        volatile double maxRadius_;

        std::vector<std::vector<cv::Point> > contours;

        cv::Mat cam, hsv, mask, bp, binsearch;
        cv::MatND hist;

        double fillratio_;

        cv::KalmanFilter kf;

        cv::Mat measurement;
};
