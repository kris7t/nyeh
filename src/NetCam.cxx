#include "NetCam.hxx"

class CamPacket : public Packet {
public:
    CamPacket(const std::vector<char> & ptr) : jpeg(ptr, true) {}
    CamPacket(const cv::Mat & jpeg) : jpeg(jpeg) {}

    int pid() const { return 0x10; }
    Char_ write() const {
        Char_ ret(new CharVect());
        ret->resize(jpeg.rows);
        memcpy(&(*ret)[0], jpeg.ptr(0), jpeg.rows);
        return ret;
    }

    cv::Mat jpeg;
};

class CamPacketReader : public PacketReader {
public:
    CamPacketReader() : PacketReader(0x10) {}
    Packet_ read(const std::vector<char> & ptr) {
        std::tr1::shared_ptr<CamPacket> p(new CamPacket(ptr));
        return p;
    };
};
static CamPacketReader reader;

NetClientCam::NetClientCam(const std::string & host) : sock(host, 14599) {
    frame_.create(480, 640, CV_8UC3);
}

void NetClientCam::grabImage() {
    std::tr1::shared_ptr<CamPacket> pn, p;
    while (pn = std::tr1::dynamic_pointer_cast<CamPacket>(sock.recv())) {
        p = pn;
    }
    if (p) {
        cv::Mat cfrm = cv::imdecode(p->jpeg, 1);
        if (cfrm.dims > 0 && (cfrm.cols > 0 && cfrm.rows > 0)) {
            jpeg_ = p->jpeg;
            frame_ = cfrm;
        }
    }
}

const cv::Mat & NetClientCam::jpeg() const {
    return jpeg_;
}
const cv::Mat & NetClientCam::frame() const {
    return frame_;
}

NetServerCam::NetServerCam() : srv(14599) {
}

void NetServerCam::push(const cv::Mat& jpeg) {
    NetSocket_ s = srv.accept();
    if (s) sock = s;
    if (sock) {
        CamPacket pck(jpeg);
        sock->send(pck);
    }
}
