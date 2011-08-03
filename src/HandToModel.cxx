#include "HandToModel.hxx"

HandToModel_ HandToModel::create(Tube tube) {
    return HandToModel_(new HandToModel(tube));
}

HandToModel::HandToModel(Tube tube)
    : tube_(tube),
      calibration_(HandToModel::defaultCalibration) {
}

void HandToModel::update(HandFilter_ hand) {
    cv::Point3f pos = hand->position();
    cv::Point3f vel = hand->velocity();
    position_.x = std::min(std::max(((pos.x - 10) / 140 - 1), -1.f), 1.f)
        * (tube_.halfSize.width - handSize);
    position_.z = -std::min(std::max(((pos.y - 10) / 100 - 1), -1.f), 1.f)
        * (tube_.halfSize.height - handSize);
    if (pos.z < calibration_.minRadius) {
        position_.y = tube_.handMax - tube_.handMovement;
    } else if (pos.z > calibration_.maxRadius) {
        position_.y = tube_.handMax;
    } else {
        double lambda = (calibration_.maxRadius / pos.z - 1.0) * calibration_.kappa;
        position_.y = tube_.handMax - tube_.handMovement * lambda;
    }
    velocity_.x = vel.x / 140 * (tube_.halfSize.width - handSize);
    velocity_.z = vel.y / 100 * (tube_.halfSize.height - handSize);
    double phi = calibration_.maxRadius * tube_.handMovement * calibration_.kappa;
    velocity_.y = phi / (pos.z * pos.z) * vel.z;
}
