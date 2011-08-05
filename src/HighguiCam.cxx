#include "HighguiCam.hxx"

HighguiCam::HighguiCam(cv::Size size, int device) : Cam(size), cap_(device) {
    cap_.set(CV_CAP_PROP_FRAME_WIDTH, size.width);
    cap_.set(CV_CAP_PROP_FRAME_HEIGHT, size.height);
    cap_.set(CV_CAP_PROP_FPS, 25);
    if (!cap_.isOpened())
        throw std::string("can't open device");
}

void HighguiCam::grabImage() {
    cap_ >> frame_;
    std::vector<uchar> ret;
    cv::imencode(".jpg", frame_, ret);
    jpeg_ = cv::Mat(ret);
}

const cv::Mat & HighguiCam::jpeg() const {
    return jpeg_;
}

const cv::Mat & HighguiCam::frame() const {
    return frame_;
}
