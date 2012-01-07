# Nyeh, a game with 3D and computer vision

## Playing

For playing Nyeh, you will need

1. A partner to play with (AI opponent is not currently implemented),
2. A round object with uniform color that stands out of the background,
3. A webcam.

If your webcam supports jpeg compression of frames, the game will
probably be more smooth and enjoyable. Otherwise, a reasonably fast,
multicore CPU is required.

You provide your opponent with your IP address and ensure that ports
*14598* and *14599* are open on your router (if you have one).
Then issue

    bin/nyeh.x

You opponent should issue

    bin/nyeh.x -h <your ip/hostname>

then follow the instructions. The game will begin shortly and last until
someone loses all their lives.

For program options, issue

    bin/nyeh.x --help

## Building

Nyeh needs the following dependencies to compile successfully:

* Rake
* GCC 4.4 or newer (for C++0x support)
* Boost 1.4x or newer
* GLEW
* GLFW
* OpenCV 2.3

For a nice GUI, compile OpenCV with the `-D WITH_QT=ON` flag.

To build Nyeh, just issue

    rake

in the root directory of your cloned repository.

The program executable is `bin/nyeh.x`.

## Contribution

Just fork the repository on GitHub and send pull requests.

The code quality may be nightmarish, and don't expect any
documentation, either. Refer to the next section for reasons. :)

## Background

Nyeh started as a project on a high school summer hackaton.
When the hackaton started, everyone was given a vividly colored
"stress-ball", in case later events turn out to be extremely stressful.
We made a game which used these balls as a method of interfacing with
the user.
