#include "HighguiCam.hxx"
#include "UvcCam.hxx"

Cam_ Cam::create(int camId) {
    Cam_ c;
    try {
        c.reset(new UvcCam(camId));
    } catch (const std::exception & e) {
        std::cerr << "fallback to highgui camera (may be slow and shitty)"
                  << std::endl;
        std::cerr << "what(): " << e.what() << std::endl;
        c.reset(new HighguiCam(camId));
    }
    return c;
}
