#include "NetCam.hxx"

class CamPacket : public Packet {
public:
    CamPacket(const std::string & ptr) : jpeg(ptr.size(), 1, CV_8U) {
        memcpy(jpeg.data, ptr.data(), ptr.size());
    }
    CamPacket(const cv::Mat & jpeg) : jpeg(jpeg) {}

    int pid() const { return 0x10; }
    Char_ write() const {
        Char_ ret(new std::string(reinterpret_cast<const char *>(jpeg.ptr(0)), jpeg.rows));
        return ret;
    }

    cv::Mat jpeg;
};

NET_READER(CamPacket, 0x10);

// -----

NetCam::NetCam() : Cam(cv::Size()), conn(14599) {
	frame_.create(240, 320, CV_8UC3);
}
NetCam::NetCam(const std::string & host) : Cam(cv::Size()), conn(host, 14599) {
	frame_.create(240, 320, CV_8UC3);
}

void NetCam::grabImage() {
	NetSocket_ s = conn.sock();
	if (!s) return;

    std::tr1::shared_ptr<CamPacket> pn, p;
    while (pn = std::tr1::dynamic_pointer_cast<CamPacket>(s->recv())) {
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

void NetCam::push(const cv::Mat& jpeg) {
	NetSocket_ s = conn.sock();
    if (s) {
        CamPacket pck(jpeg);
        s->send(pck);
    }
}

const cv::Mat & NetCam::jpeg() const {
    return jpeg_;
}
const cv::Mat & NetCam::frame() const {
    return frame_;
}
