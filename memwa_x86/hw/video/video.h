#ifndef _VIDEO_H_
#define _VIDEO_H_

#include "if.h"
#include "glfw3.h"

#ifdef __cplusplus
extern "C" {
#endif

GLFWwindow *video_init();
void video_run(uint8_t *frame_p);

#ifdef __cplusplus
}
#endif
#endif
