#pragma once

class Elapsed {
    public:
        Elapsed();
        void reset();
        operator double();
    private:
        timespec start_;
};
