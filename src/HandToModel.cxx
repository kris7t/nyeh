#include "HandToModel.hxx"

HandToModel_ HandToModel::create(Tube tube) {
    return HandToModel_(new HandToModel(tube));
}

HandToModel::HandToModel(Tube tube) : tube_(tube) {
}

void HandToModel::update(Hand_ hand) {
    cv::Point3f pos = hand->position();
    cv::Point3f vel = hand->velocity();
    position_.x = std::min(std::max((pos.x / 160 - 1), -1.f), 1.f) * tube_.halfSize.width;
    position_.z = -std::min(std::max((pos.y / 120 - 1), -1.f), 1.f) * tube_.halfSize.height; 
    if (pos.z < hand->minRadius()) {
        position_.y = tube_.handMin;
    } else if (pos.z > hand->maxRadius()) {
        position_.y = tube_.handMax;
    } else {
        double lambda = (hand->maxRadius() / pos.z - 1.0) / hand->kappa();
        position_.y = tube_.handMax + (tube_.handMin - tube_.handMax) * lambda;
    }
    velocity_.x = vel.x / 160 * tube_.halfSize.width;
    velocity_.z = -vel.y / 120 * tube_.halfSize.height;
    velocity_.y = 0;
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
