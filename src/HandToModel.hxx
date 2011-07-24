#pragma once

#include "Hand.hxx"
#include "model.hxx"

class HandToModel;
typedef std::tr1::shared_ptr<HandToModel> HandToModel_;

class HandToModel {
    public:
        HandToModel(Tube tube);
        void update(Hand_ hand);
        cv::Point3f position() volatile const;
        cv::Point3f velocity() volatile const;
        static HandToModel_ create(Tube tube);
    private:
        Tube tube_;
        volatile cv::Point3f position_, velocity_;
};
