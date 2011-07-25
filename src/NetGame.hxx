#pragma once

#include "Net.hxx"
#include "model.hxx"

class NetGame {
public:
    NetGame();
    NetGame(const std::string & host);

    void sync(Balls & balls, GameState & state, const Tube & tube);

private:
    NetConnection conn;
};
typedef std::tr1::shared_ptr<NetGame> NetGame_;
