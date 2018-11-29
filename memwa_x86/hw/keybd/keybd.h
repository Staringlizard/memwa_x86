#ifndef _KEYBD_H
#define _KEYBD_H

#include "if.h"
#include "glfw3.h"

#define DEFAULT_KEY_MAX    71

void keybd_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
void keybd_pop_map(uint8_t *conf_text, if_keybd_map_t *keybd_map_p);
if_keybd_map_t *keybd_get_default_map();

#endif
