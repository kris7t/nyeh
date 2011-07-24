#include "NetGame.hxx"

inline float htobef(float host) {
    uint32_t be = htobe32(*reinterpret_cast<uint32_t *>(&host));
    return *reinterpret_cast<float *>(&be);
}


inline float betohf(float be) {
    uint32_t host = htobe32(*reinterpret_cast<uint32_t *>(&be));
    return *reinterpret_cast<float *>(&host);
}

struct GamePacketStruct {
    GamePacketStruct(uint64_t id, const Ball & b)
        : id(id), type(b.type),
          px(b.position.x), py(b.position.y), pz(b.position.z),
          vx(b.velocity.x), vy(b.velocity.y), vz(b.velocity.z) {}
    GamePacketStruct(const void * ptr) {
        memcpy(this, ptr, sizeof(GamePacketStruct));
        fromNet();
    }

    void toNet() {
        id = htobe64(id);
        type = htobe32(type);
        px = htobef(px);
        py = htobef(py);
        pz = htobef(pz);
        vx = htobef(vx);
        vy = htobef(vy);
        vz = htobef(vz);
    }

    void fromNet() {
        id = be64toh(id);
        type = be32toh(type);
        px = betohf(px);
        py = betohf(py);
        pz = betohf(pz);
        vx = betohf(vx);
        vy = betohf(vy);
        vz = betohf(vz);
    }

    void apply(Balls & balls, const Tube & tube) {
        Ball & b = balls[id];
        b.type = type;
		float y = 2 * tube.separator - py;
        b.position = cv::Point3f(-px, y, pz);
        b.velocity = cv::Point3f(-vx, -vy, vz);
        b.owner = ballOwnerRemote;
    }

    uint64_t id;
    int32_t type;
    float px, py, pz, vx, vy, vz;
} NOALIGN;

class GamePacket : public Packet {
public:
    GamePacket(const Balls & balls) {
        for (Balls::const_iterator it = balls.begin(); it != balls.end(); ++it) {
            if (it->second.owner == ballOwnerLocal) {
                data.push_back(GamePacketStruct(it->first, it->second));
                data.back().toNet();
            }
        }
    }
    GamePacket(const CharVect & ptr) {
        size_t s = ptr.size() / sizeof(GamePacketStruct);
        for (size_t i = 0; i < s; ++i) {
            data.push_back(GamePacketStruct(&ptr[i*sizeof(GamePacketStruct)]));
        }
    }

    int pid() const { return 0x20; }
    Char_ write() const {
        Char_ ret(new CharVect(data.size() * sizeof(GamePacketStruct)));

        for (size_t i = 0; i < data.size(); ++i) {
            memcpy(&(*ret)[i * sizeof(GamePacketStruct)], &data[i], sizeof(GamePacketStruct));
        }
        return ret;
    };

    void apply(Balls & balls, const Tube & tube) {
        for (std::vector<GamePacketStruct>::iterator it = data.begin();
             it != data.end(); ++it) {
            it->apply(balls, tube);
        }
    }

private:
    std::vector<GamePacketStruct> data;
};

class GamePacketReader : public PacketReader {
public:
    GamePacketReader() : PacketReader(0x20) {}
    Packet_ read(const CharVect & ptr) {
        return Packet_(new GamePacket(ptr));
    }
};
static GamePacketReader reader;

//

NetGame::NetGame() : conn(14598) {}
NetGame::NetGame(const std::string & host) : conn(host, 14598) {}

void NetGame::sync(Balls & balls, const Tube & tube) {
    NetSocket_ s = conn.sock();
    if (!s) return;

    s->send(GamePacket(balls));

    while (auto pk = std::tr1::dynamic_pointer_cast<GamePacket>(s->recv())) {
        pk->apply(balls, tube);
    }
}
