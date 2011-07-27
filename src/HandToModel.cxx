#include "HandToModel.hxx"

HandToModel_ HandToModel::create(Tube tube) {
    return HandToModel_(new HandToModel(tube));
}

HandToModel::HandToModel(Tube tube) : tube_(tube) {
}

void HandToModel::update(Hand_ hand) {
    cv::Point3f pos = hand->position();
    cv::Point3f vel = hand->velocity();
    position_.x = std::min(std::max(((pos.x - 10) / 140 - 1), -1.f), 1.f) * (tube_.halfSize.width - handSize);
    position_.z = -std::min(std::max(((pos.y - 10) / 100 - 1), -1.f), 1.f) * (tube_.halfSize.height - handSize);
    if (pos.z < hand->minRadius()) {
        position_.y = tube_.handMax - tube_.handMovement;
    } else if (pos.z > hand->maxRadius()) {
        position_.y = tube_.handMax;
    } else {
        double lambda = (hand->maxRadius() / pos.z - 1.0) / hand->kappa();
        position_.y = tube_.handMax - tube_.handMovement * lambda;
    }
    velocity_.x = vel.x / 140 * tube_.halfSize.width;
    velocity_.z = -vel.y / 100 * tube_.halfSize.height;
    double phi = hand->maxRadius() * tube_.handMovement / hand->kappa();
    velocity_.y = phi / (pos.z * pos.z) * vel.z;
}

cv::Point3f HandToModel::position() volatile const {
    cv::Point3f p;
    p.x = position_.x;
    p.y = position_.y;
    p.z = position_.z;
    return p;
}

cv::Point3f HandToModel::velocity() volatile const {
    cv::Point3f p;
    p.x = velocity_.x;
    p.y = velocity_.y;
    p.z = velocity_.z;
    return p;
}
