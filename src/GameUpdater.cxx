#include <GameUpdater.hxx>

GameUpdater::GameUpdater(Tube tube)
    : tube_(tube) {
    
}

static void doCollision(Ball & a, Ball & b) {
    cv::Point3f diff;
    diff.x = a.position.x - b.position.x;
    diff.y = a.position.y - b.position.y;
    diff.z = a.position.z - b.position.z;
    float norm = cv::norm(diff);
    diff.x /= norm;
    diff.y /= norm;
    diff.z /= norm;
    float dota = a.velocity.x * diff.x
        + a.velocity.y * diff.y
        + a.velocity.z * diff.z;
    float dotb = b.velocity.x * diff.x
        + b.velocity.y * diff.y
        + b.velocity.z * diff.z;
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

void GameUpdater::tick(double dt, Balls & balls) const {
    for (Balls::iterator it = balls.begin();
            it != balls.end(); ++it) {
        it->position.x += it->velocity.x * dt;
        it->position.y += it->velocity.y * dt;
        it->position.z += it->velocity.z * dt;
        if (it->position.y < tube_.goal) {
            Balls::iterator del = it;
            --it;
            balls.erase(del);
            std::cout << "\a" << std::flush;
            continue;
        }
        if (std::abs(it->position.x) >= tube_.halfSize.width) {
            it->velocity.x *= -1;
        }
        if (std::abs(it->position.z) >= tube_.halfSize.height) {
            it->velocity.z *= -1;
        }
    }
    for (Balls::iterator a = balls.begin();
            a != balls.end(); ++a) {
        for (Balls::iterator b = balls.begin();
                b != a; ++b) {
            if (cv::norm(a->position - b->position) <= .25f) {
                doCollision(*a, *b);
            }
        }
    }
}
