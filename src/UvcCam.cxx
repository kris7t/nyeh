#include "UvcCam.hxx"

UvcCam::UvcCam(cv::Size size, int device) : Cam(size) {
    memset(map, 0, sizeof(void *) * BUFFERS);

    std::string str = (boost::format("/dev/video%d") % device).str();
    fd = v4l2_open(str.c_str(), O_RDWR);
    if (fd < 0)
        throw std::runtime_error("Couldn't open video device");

    v4l2_capability cap;
    memset(&cap, 0, sizeof(v4l2_capability));
    if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        v4l2_close(fd);
        throw std::runtime_error("ioctl kaboom!!!");
    }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
        !(cap.capabilities & V4L2_CAP_STREAMING)) {
        v4l2_close(fd);
        throw std::runtime_error("Ur cam iz shitty");
    }

    v4l2_format fmt;
    memset(&fmt, 0, sizeof(v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = size.width;
    fmt.fmt.pix.height = size.height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    errno = 0;
    if (v4l2_ioctl(fd, VIDIOC_S_FMT, &fmt)) {
        perror("v4l2_ioctl");
        v4l2_close(fd);
        throw std::runtime_error("Can't set format");
    }
    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_JPEG
            && fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG) {
        v4l2_close(fd);
        throw std::runtime_error("jpg not supported");
    }

    v4l2_requestbuffers rq;
    memset(&rq, 0, sizeof(v4l2_requestbuffers));
    rq.count = BUFFERS;
    rq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rq.memory = V4L2_MEMORY_MMAP;
    if (v4l2_ioctl(fd, VIDIOC_REQBUFS, &rq)) {
        v4l2_close(fd);
        throw std::runtime_error("bufferalloc failed");
    }

    v4l2_buffer buf;
    for (size_t i = 0; i < BUFFERS; ++i) {
        memset(&buf, 0, sizeof(v4l2_buffer));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (v4l2_ioctl(fd, VIDIOC_QUERYBUF, &buf)) {
            v4l2_close(fd);
            throw std::runtime_error("buffer queue fail'd");
        }

        len = buf.length;
        map[i] = mmap(0, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                      buf.m.offset);
        if (map[i] == MAP_FAILED) {
            destroy();
            throw std::runtime_error("mmap fail'd");
        }
    }

    for (size_t i = 0; i < BUFFERS; ++i) {
        memset(&buf, 0, sizeof(v4l2_buffer));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (v4l2_ioctl(fd, VIDIOC_QBUF, &buf)) {
            destroy();
            throw std::runtime_error("queue buffer fail'd");
        }
    }

    v4l2_streamparm fps;
    memset(&fps, 0, sizeof(v4l2_streamparm));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fps.parm.capture.timeperframe.numerator = 1;
    fps.parm.capture.timeperframe.denominator = 25;
    v4l2_ioctl(fd, VIDIOC_S_PARM, fps);

    v4l2_queryctrl ctrl;
    for (int i = V4L2_CID_PRIVATE_BASE;; ++i) {
        ctrl.id = i;
        if (v4l2_ioctl(fd, VIDIOC_QUERYCTRL, &ctrl))
            break;
        if (ctrl.type == V4L2_CTRL_TYPE_BOOLEAN &&
            strncasecmp(reinterpret_cast<char *>(ctrl.name), "auto", 4)) {
            std::cerr << "Automatically disabling private auto thing: "
                      << ctrl.name << std::endl;
            disable(reinterpret_cast<char *>(ctrl.name), ctrl.id);
        } else {
            std::cerr << ctrl.name << "; " << ctrl.type << std::endl;
        }
    }
#define CDISABLE(x) disable(#x, x)
    CDISABLE(V4L2_CID_AUTO_WHITE_BALANCE);
    CDISABLE(V4L2_CID_AUTOGAIN);
    CDISABLE(V4L2_CID_HUE_AUTO);
    CDISABLE(V4L2_CID_AUTOBRIGHTNESS);
#undef CDISABLE

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_ioctl(fd, VIDIOC_STREAMON, &type)) {
        destroy();
        throw std::runtime_error("can't start capture");
    }

    frame_.create(this->size(), CV_8UC3);
}

void UvcCam::disable(const std::string& name, int ctrlid) {
    v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(v4l2_control));
    ctrl.id = ctrlid;
    ctrl.value = true;
    if (v4l2_ioctl(fd, VIDIOC_S_CTRL, &ctrl)) {
        std::cerr << "Can't disable control " << name << " (" << ctrlid
                  << ") (maybe not supported?)" << std::endl;
    }
}

UvcCam::~UvcCam() {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_ioctl(fd, VIDIOC_STREAMOFF, &type);
    destroy();
}

const cv::Mat & UvcCam::jpeg() const {
    return jpeg_;
}

const cv::Mat & UvcCam::frame() const {
    return frame_;
}

void UvcCam::grabImage() {
    v4l2_buffer buf;
    memset(&buf, 0, sizeof(v4l2_buffer));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (v4l2_ioctl(fd, VIDIOC_DQBUF, &buf)) {
        throw std::string("buffer dequeue fail'd");
    }

    if (buf.bytesused > 0xaf) {
        cv::Mat cjpg(buf.bytesused, 1, CV_8U);
        memcpy(cjpg.ptr(0), map[buf.index], buf.bytesused);
        cv::Mat cfrm = cv::imdecode(cjpg, 1);

        if (cfrm.dims > 0 &&
            //(cfrm.type() & CV_MAGIC_MASK) == CV_MAT_MAGIC_VAL &&
            (cfrm.cols > 0 && cfrm.rows > 0)) {
            jpeg_ = cjpg;
            frame_ = cfrm;
        }
    }

    if (v4l2_ioctl(fd, VIDIOC_QBUF, &buf)) {
        throw std::string("buffer queue fail'd");
    }
}

void UvcCam::destroy() {
    for (size_t i = 0; i < BUFFERS; ++i)
        if (map[i])
            munmap(map[i], len);
    v4l2_close(fd);
}
