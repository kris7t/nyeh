#include <GameUpdater.hxx>

GameUpdater::GameUpdater(cv::Size2f tubeDimsHalf)
    : tubeDimsHalf_(tubeDimsHalf) {
    
}

static void doCollosion(Ball & a, Ball & b) {
    cv::Point3f diff;
    diff.x = a.position.x - b.position.x;
    diff.y = a.position.y - b.position.y;
    diff.z = a.position.z - b.position.z;
    float norm = cv::norm(diff);
    if (norm >= 1.0e-5) {
        diff.x /= norm;
        diff.y /= norm;
        diff.z /= norm;
        float dota = a.velocity.x * diff.x
            + a.velocity.y * diff.y
            + a.velocity.z * diff.z;
        float dotb = b.velocity.x * diff.x
            + b.velocity.y * diff.y
            + b.velocity.z * diff.z;
        a.velocity.x -= 2 * dota * diff.x;
        a.velocity.y -= 2 * dota * diff.y;
        a.velocity.z -= 2 * dota * diff.z;
        b.velocity.x += 2 * dotb * diff.x;
        b.velocity.y += 2 * dotb * diff.y;
        b.velocity.z += 2 * dotb * diff.z;
    } else {
        a.velocity.x *= -1;
        a.velocity.y *= -1;
        a.velocity.z *= -1;
        b.velocity.x *= -1;
        b.velocity.y *= -1;
        b.velocity.z *= -1;
    }
}

void GameUpdater::tick(double dt, Balls & balls) const {
    for (Balls::iterator it = balls.begin();
            it != balls.end(); ++it) {
        it->position.x += it->velocity.x * dt;
        it->position.y += it->velocity.y * dt;
        it->position.z += it->velocity.z * dt;
        if (it->position.y < 3.0f) {
            Balls::iterator del = it;
            --it;
            balls.erase(del);
        }
        if (std::abs(it->position.x) >= tubeDimsHalf_.width) {
            it->velocity.x *= -1;
        }
        if (std::abs(it->position.z) >= tubeDimsHalf_.height) {
            it->velocity.z *= -1;
        }
    }
    for (Balls::iterator a = balls.begin();
            a != balls.end(); ++a) {
        for (Balls::iterator b = balls.begin();
                b != a; ++b) {
            if (cv::norm(a->position - b->position) <= .25f) {
                doCollosion(*a, *b);
            }
        }
    }
}
