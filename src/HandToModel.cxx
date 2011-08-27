#include "HandToModel.hxx"

HandToModel_ HandToModel::create(Tube tube) {
    return HandToModel_(new HandToModel(tube));
}

HandToModel::HandToModel(Tube tube)
    : tube_(tube),
      calibration_(HandToModel::defaultCalibration) {
}

void HandToModel::update(const HandFilter_ & hand) {
    float xAbsMax = tube_.halfSize.width - handSize,
        zAbsMax = tube_.halfSize.height - handSize;
    cv::Point3f pos = hand->position();
    cv::Point3f vel = hand->velocity();
    position_.x = std::min(std::max(
                (pos.x - calibration_.offset.x) / calibration_.halfSize.width - 1,
                -1.f), 1.f) * xAbsMax;
    position_.z = -std::min(std::max(
                (pos.y - calibration_.offset.y) / calibration_.halfSize.height - 1,
                -1.f), 1.f) * zAbsMax;
    if (pos.z < calibration_.minRadius) {
        position_.y = tube_.handMax - tube_.handMovement;
    } else if (pos.z > calibration_.maxRadius) {
        position_.y = tube_.handMax;
    } else {
        double lambda = (calibration_.maxRadius / pos.z - 1.0) * calibration_.kappa;
        position_.y = tube_.handMax - tube_.handMovement * lambda;
    }
    velocity_.x = vel.x / calibration_.halfSize.width * xAbsMax;
    velocity_.z = vel.y / calibration_.halfSize.height * zAbsMax;
    double phi = calibration_.maxRadius * tube_.handMovement * calibration_.kappa;
    velocity_.y = phi / (pos.z * pos.z) * vel.z;
}

void HandToModel::extrapolate(double dt) {
    position_.x += velocity_.x;
    position_.y += velocity_.y;
    position_.z += velocity_.z;
}
