#include "HighguiCam.hxx"
#include "UvcCam.hxx"

std::tr1::shared_ptr<Cam> Cam::create(int camId) {
    std::tr1::shared_ptr<Cam> c;
    try {
        c.reset(new UvcCam(camId));
    } catch (const std::exception& e) {
        std::cerr << "fallback to highgui camera (may be slow and shitty)"
                  << std::endl;
        c.reset(new HighguiCam(camId));
    }
    return c;
}
