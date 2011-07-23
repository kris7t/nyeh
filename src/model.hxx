#pragma once

struct Tube {
    cv::Size2f halfSize;
    float goal, opponentGoal, handMin, handMax, separator;
};

enum BallOwner {
    ballOwnerLocal,
    ballOwnerRemote
};

struct Ball {
    int type;
    cv::Point3f position, velocity;
    BallOwner owner;
};

typedef std::map<uint64_t, Ball> Balls;
