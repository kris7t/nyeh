#pragma once

#include "Net.hxx"
#include "model.hxx"

class NetGame {
public:
    NetGame();
    NetGame(const std::string & host);

    void sync(Balls & balls);

private:
    NetConnection conn;
};
