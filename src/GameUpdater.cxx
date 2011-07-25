#include <GameUpdater.hxx>

static const float eps = 1e-4;

GameUpdater::GameUpdater(Tube tube)
    : tube_(tube), seed_(time(NULL)) {
}

static cv::Point3f normDiff(cv::Point3f a, cv::Point3f b, float * normOut) {
    cv::Point3f diff = a - b;
    float norm = cv::norm(diff);
    diff.x /= norm;
    diff.y /= norm;
    diff.z /= norm;
    if (normOut) {
        *normOut = norm;
    }
    return diff;
}

static float dot(cv::Point3f a, cv::Point3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static void doCollision(Ball & a, Ball & b) {
    float norm;
    cv::Point3f diff = normDiff(a.position, b.position, &norm);
    if (!(norm <= .25f) || norm < eps) {
        return;
    }
    float dota = dot(a.velocity, diff);
    float dotb = dot(b.velocity, diff);
    if (dotb < dota) {
        return;
    }
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
    float norm;
    cv::Point3f diff = normDiff(ball.position, hp, &norm);
    if (!(norm <= .55f) || norm < eps) {
        return;
    }
    float vball = dot(ball.velocity, diff),
          vhand = dot(hv, diff);
    if (vhand < vball) {
        return;
    }
    float change = 2 * (vhand - vball);
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
        if (it->second.owner != ballOwnerLocal) {
            continue;
        }
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
        if (it->second.position.y > tube_.separator + eps) {
            it->second.owner = ballOwnerRemote;
        }
    }
    for (Balls::iterator a = balls.begin();
            a != balls.end(); ++a) {
        if (a->second.owner != ballOwnerLocal) {
            continue;
        }
        for (Balls::iterator b = balls.begin();
                b != a; ++b) {
            if (b->second.owner != ballOwnerLocal) {
                continue;
            }
            doCollision(a->second, b->second);
        }
    }
}

void GameUpdater::randomizeBall(Ball & ball) {
    ball.position.x = (static_cast<float>(rand_r(&seed_)) / RAND_MAX * 2 - 1)
        * tube_.halfSize.width;
    ball.position.z = (static_cast<float>(rand_r(&seed_)) / RAND_MAX * 2 - 1)
        * tube_.halfSize.height;
    ball.position.y = tube_.separator - eps
        - static_cast<float>(rand_r(&seed_)) / RAND_MAX * tube_.spawnArea;
}
