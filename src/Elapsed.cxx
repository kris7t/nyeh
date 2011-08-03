#include "Elapsed.hxx"

Elapsed::Elapsed() {
    reset();
}

void Elapsed::reset() {
    clock_gettime(CLOCK_MONOTONIC, &start_);
}

Elapsed::operator double() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    double diff = now.tv_sec - start_.tv_sec;
    diff += static_cast<double>(now.tv_nsec - start_.tv_nsec) * 1e-9;
    start_ = now;
    return diff;
}
