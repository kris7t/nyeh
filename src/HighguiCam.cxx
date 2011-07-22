#include "HighguiCam.hxx"

HighguiCam::HighguiCam(int device) : cap_(device) {
    cap_.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap_.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
}

void HighguiCam::grabImage() {
    cap_ >> frame_;
    std::vector<uchar> ret;
    cv::imencode(".jpg", frame_, ret);
    jpeg_ = cv::Mat(ret);
}

const cv::Mat& HighguiCam::jpeg() const {
    return jpeg_;
}

const cv::Mat& HighguiCam::frame() const {
    return frame_;
}
