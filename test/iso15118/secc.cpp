#include <cstdio>

#include <eviso15118/tbd_controller.hpp>

int main(int argc, char* argv[]) {
    eviso15118TbdController controller;
    controller.loop();

    return 0;
}
