#pragma once

#include "Hand.hxx"

class HistogramHand : public Hand {
    public:
        HistogramHand(double thresh, double fillratio, double projection);
        virtual void calibrate(Cam_ cam);
        virtual void update(const cv::Mat & frame);
        virtual volatile const cv::Point3f & position() volatile const;
        virtual volatile  const cv::Vec3f & velocity() volatile const;

    private:
        void measureHist(Cam_ cap, cv::MatND & hist, cv::Point center, int radius, bool accumulate = true);

        volatile cv::Point3f position_;
        volatile cv::Vec3f velocity_;

        std::vector<std::vector<cv::Point> > contours;

        cv::Mat cam, hsv, mask, bp, binary, maskedbinary;
        cv::MatND hist;

        double thresh_, fillratio_, projection_;
};
