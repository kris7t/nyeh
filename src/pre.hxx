#ifndef __PRE_HXX
#define __PRE_HXX

#include <cctype>
#include <ctime>
#include <cstring>

#include <iostream>
#include <vector>
#include <list>
#include <tr1/memory>

// OpenCV headers
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

// OpenGL headers
#include <GL/glew.h>
#include <GL/glfw.h>
#include <GL/glu.h>

// v4l
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <libv4l2.h>
#include <fcntl.h>

// boost
#include <boost/utility.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>

// socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#endif
