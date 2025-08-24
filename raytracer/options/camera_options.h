#pragma once

#include "vector.h"

#include <numbers>

struct CameraOptions {
    int screen_width;
    int screen_height;
    double fov = std::numbers::pi / 2;
    Vector look_from = {0., 0., 0.};
    Vector look_to = {0., 0., -1.};

    // CameraOptions(int width, int height, double fov = std::numbers::pi / 2,
    //               Vector look_from = {0., 0., 0.}, 
    //               Vector look_to = {0., 0., -1.})
    //     : screen_width(width),
    //       screen_height(height),
    //       fov(fov),
    //       look_from(look_from),
    //       look_to(look_to) {
    // }
};