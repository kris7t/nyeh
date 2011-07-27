#pragma once

struct Tube {
    cv::Size2f halfSize;
    float goal, opponentGoal, handMovement, handMax, separator, spawnArea;
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

struct BallType {
    float size, r, g, b;
    int value;
};

extern const BallType ballTypes[];

extern const float handSize;

struct GameState {
    uint8_t own_lives;
    uint8_t opponent_lives;
    double own_ratio;
};
