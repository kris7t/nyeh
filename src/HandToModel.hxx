#pragma once

#include "HandFilter.hxx"
#include "model.hxx"

class HandToModel;
typedef std::tr1::shared_ptr<HandToModel> HandToModel_;

class HandToModel {
    public:
        static const struct Calibration {
            float minRadius, maxRadius, kappa;
        } defaultCalibration;

        static HandToModel_ create(Tube tube);

        HandToModel(Tube tube);
        void update(HandFilter_ hand);
        inline const cv::Point3f & position() const {
            return position_;
        }
        inline const cv::Point3f & velocity() const {
            return velocity_;
        }
        inline const Calibration & calibration() const {
            return calibration_;
        }
        inline void calibration(const Calibration & value) {
            calibration_ = value;
        }
    private:
        Tube tube_;
        Calibration calibration_;
        cv::Point3f position_, velocity_;
};
