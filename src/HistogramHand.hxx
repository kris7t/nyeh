#pragma once

#include "Hand.hxx"

class HistogramHand : public Hand {
    public:
        HistogramHand(double thresh, double fillratio, double t);
        virtual void calibrate(Cam_ cam);
        virtual void update(const cv::Mat & frame);
        virtual const cv::Point3f & position() const;
        virtual const cv::Point3f & velocity() const;    

    private:
        void measureHist(Cam_ cap, cv::MatND & hist, cv::Point center, int radius, bool accumulate = true);

        cv::Point3f position_;
        cv::Point3f velocity_;

        std::vector<std::vector<cv::Point> > contours;

        cv::Mat cam, hsv, mask, bp, binary, maskedbinary;
        cv::MatND hist;

        double thresh_, fillratio_, projection_;

        cv::KalmanFilter kf;

        cv::Mat measurement;
};
