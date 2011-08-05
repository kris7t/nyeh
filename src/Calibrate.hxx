#pragma once

#include "Cam.hxx"
#include "HistogramHand.hxx"
#include "HandFilter.hxx"
#include "HandToModel.hxx"

class Calibrate;
typedef std::tr1::shared_ptr<Calibrate> Calibrate_;

class Calibrate {
    public:
        static Calibrate_ create();

        Calibrate();
        inline void apply(Hand_ target) const {
            target->calibration(handCalibration_);
        }
        inline void apply(HandFilter_ target) const {
            target->calibration(filterCalibration_);
        }
        inline void apply(HandToModel_ target) const {
            target->calibration(toModelCalibration_);
        }
        void run(Cam_ cap, Hand_ hand, HandFilter_ filter, HandToModel_ toModel);
    private:
        void measureHist(Cam_ cap, cv::Point2f center, float radius, bool accumulate = true);
        double measureRadius(Cam_ cap, Hand_ hand, HandFilter_ filter);
        cv::Point2f measurePosition(Cam_ cap, Hand_ hand, HandFilter_ filter);
        cv::Point3f measurePositionAndRadius(Cam_ cap, Hand_ hand, HandFilter_ filter);
        void writableFlippedShot(Cam_ cap);
        void guiHist(Cam_ cap);
        void guiRadius(Cam_ cap, Hand_ hand, HandFilter_ filter);
        void guiBounds(Cam_ cap, Hand_ hand, HandFilter_ filter);
        void drawCircle(cv::Point2f center, double radius);
        void imshow();

        HistogramHand::Calibration handCalibration_;
        HandFilter::Calibration filterCalibration_;
        HandToModel::Calibration toModelCalibration_;
        const float * theRanges_[3];
        cv::Mat mask_, frame_, hsv_;
};
