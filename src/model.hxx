#pragma once

struct Tube {
    cv::Size2f halfSize;
    float goal, handMin, handMax;
};

enum BallOwner {
    ballOwnerLocal,
    ballOwnerRemote
};

struct Ball {
    uint64_t id;
    int type;
    cv::Point3f position, velocity;
    BallOwner owner;
};

typedef std::list<Ball> Balls;
