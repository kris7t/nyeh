#include "HighguiCam.hxx"
#include "UvcCam.hxx"

Cam_ Cam::create(cv::Size size, int camId) {
    Cam_ c;
    try {
        c.reset(new UvcCam(size, camId));
    } catch (const std::exception & e) {
        std::cerr << "fallback to highgui camera (may be slow and shitty)"
                  << std::endl;
        std::cerr << "what(): " << e.what() << std::endl;
        c.reset(new HighguiCam(size, camId));
    }
    return c;
}

Cam::Cam(cv::Size size) : size_(size) {
}
