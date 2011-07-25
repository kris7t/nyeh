#include <GameUpdater.hxx>

static const float eps = 1e-4;

GameUpdater::GameUpdater(Tube tube)
    : tube_(tube), seed_(time(NULL)) {
}

static cv::Point3f normDiff(cv::Point3f a, cv::Point3f b) {
    cv::Point3f diff;
    diff.x = a.x - b.x;
    diff.y = a.y - b.y;
    diff.z = a.z - b.z;
    float norm = cv::norm(diff);
    diff.x /= norm;
    diff.y /= norm;
    diff.z /= norm;
    return diff;
}

static float dot(cv::Point3f a, cv::Point3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static void doCollision(Ball & a, Ball & b) {
    cv::Point3f diff = normDiff(a.position, b.position);
    float dota = dot(a.velocity, diff);
    float dotb = dot(b.velocity, diff);
    float dotdiff = dota - dotb,
          cx = dotdiff * diff.x,
          cy = dotdiff * diff.y,
          cz = dotdiff * diff.z;
    a.velocity.x -= cx;
    a.velocity.y -= cy;
    a.velocity.z -= cz;
    b.velocity.x += cx;
    b.velocity.y += cy;
    b.velocity.z += cz;
}

void doCollision(Ball & ball, HandToModel_ hand) {
    cv::Point3f hp = hand->position(),
        hv = hand->velocity();
    cv::Point3f diff = normDiff(ball.position, hp);
    float vball = dot(ball.velocity, diff),
          vhand = dot(hv, diff),
          change = 2 * (vhand - vball);
    ball.velocity.x += change * diff.x;
    ball.velocity.y += change * diff.y;
    ball.velocity.z += change * diff.z;
}

void GameUpdater::tick(double dt, Balls & balls, GameState & state, HandToModel_ hand) {
    for (Balls::iterator it = balls.begin();
            it != balls.end(); ++it) {
        it->second.position.x += it->second.velocity.x * dt;
        it->second.position.y += it->second.velocity.y * dt;
        it->second.position.z += it->second.velocity.z * dt;
        if (it->second.position.y < tube_.goal) {
            randomizeBall(it->second);
            if (state.own_lives) {
                state.own_lives -= 1;
            }
            std::cout << "\a" << std::flush;
            continue;
        }
        if (cv::norm(it->second.position -  hand->position()) <= .55f) {
            doCollision(it->second, hand);
        }
        if (std::abs(it->second.position.x) >= tube_.halfSize.width) {
            it->second.velocity.x = -copysign(it->second.velocity.x,
                    it->second.position.x);
            it->second.position.x = copysign(tube_.halfSize.width - eps,
                    it->second.position.x);
        }
        if (std::abs(it->second.position.z) >= tube_.halfSize.height) {
            it->second.velocity.z = -copysign(it->second.velocity.z,
                    it->second.position.z);
            it->second.position.z = copysign(tube_.halfSize.height - eps,
                    it->second.position.z);
        }
    }
    for (Balls::iterator a = balls.begin();
            a != balls.end(); ++a) {
        for (Balls::iterator b = balls.begin();
                b != a; ++b) {
            if (cv::norm(a->second.position - b->second.position) <= .25f) {
                doCollision(a->second, b->second);
            }
        }
    }
}

void GameUpdater::randomizeBall(Ball & ball) {
    ball.position.x = (static_cast<float>(rand_r(&seed_)) / RAND_MAX * 2 - 1)
        * tube_.halfSize.width;
    ball.position.z = (static_cast<float>(rand_r(&seed_)) / RAND_MAX * 2 - 1)
        * tube_.halfSize.height;
    ball.position.y = tube_.separator + eps
        + static_cast<float>(rand_r(&seed_)) / RAND_MAX * tube_.spawnArea;
}
