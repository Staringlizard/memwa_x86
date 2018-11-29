/*
 * memwa keyboard functions
 *
 * Copyright (c) 2018 Mathias Edman <mail@scynode.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA
 *
 */

#include <windows.h>
#include <string.h>
#include "keybd.h"
#include "main.h"

#pragma warning(disable:4996) /* Disable warnings _CRT_SECURE_NO_WARNINGS */

#define MAX_SIMUL_KEYS		6

extern "C" if_emu_cc_t g_if_cc_emu;
extern "C" if_emu_dd_t g_if_dd_emu;

typedef struct key_t
{
	int key;
	int scancode;
};

static if_keybd_map_t g_default_keybd_map_p[DEFAULT_KEY_MAX] =
{
	{0x0E,0,0} /* KEY_INSDEL */,
	{0x1C,1,0} /* KEY_RETURN */,
	{0x4B,2,0}, /* KEY_CURSOR_LR */
	{0x41,3,0}, /* KEY_F7 */
	{0x3B,4,0}, /* KEY_F1 */
	{0x3D,5,0}, /* KEY_F3 */
	{0x3F,6,0}, /* KEY_F5 */
	{0x48,7,0}, /* KEY_CURSOR_UD */
	{0x04,0,1}, /* KEY_3 */
	{0x11,1,1}, /* KEY_W */
	{0x1E,2,1}, /* KEY_A */
	{0x05,3,1}, /* KEY_4 */
	{0x2C,4,1}, /* KEY_Z */
	{0x1F,5,1}, /* KEY_S */
	{0x12,6,1}, /* KEY_E */
	/* KEY_LEFT_SHIFT */
	{0x06,0,2}, /* KEY_5 */
	{0x13,1,2}, /* KEY_R */
	{0x20,2,2}, /* KEY_D */
	{0x07,3,2}, /* KEY_6 */
	{0x2E,4,2}, /* KEY_C */
	{0x21,5,2}, /* KEY_F */
	{0x14,6,2}, /* KEY_T */
	{0x2D,7,2}, /* KEY_X */
	{0x08,0,3}, /* KEY_7 */
	{0x15,1,3}, /* KEY_Y */
	{0x22,2,3}, /* KEY_G */
	{0x09,3,3}, /* KEY_8 */
	{0x30,4,3}, /* KEY_B */
	{0x23,5,3}, /* KEY_H */
	{0x16,6,3}, /* KEY_U */
	{0x2F,7,3}, /* KEY_V */
	{0x0A,0,4}, /* KEY_9 */
	{0x17,1,4}, /* KEY_I */
	{0x24,2,4}, /* KEY_J */
	{0x0B,3,4}, /* KEY_0 */
	{0x32,4,4}, /* KEY_M */
	{0x25,5,4}, /* KEY_K */
	{0x18,6,4}, /* KEY_O */
	{0x31,7,4}, /* KEY_N */
	{0x4E,0,5}, /* KEY_PLUS */
	{0x19,1,5}, /* KEY_P */
	{0x26,2,5}, /* KEY_L */
	{0x4A,3,5}, /* KEY_MINUS */
	{0x34,4,5}, /* KEY_PERIOD */
	{0x27,5,5}, /* KEY_COLON */
	{0xFF,6,5}, /* KEY_AT */
	{0x33,7,5}, /* KEY_COMMA */
	{0xFF,0,6}, /* KEY_POUND */
	{0x37,1,6}, /* KEY_ASTERISK */
	{0x27,2,6}, /* KEY_SEMICOLON */
	{0xFF,3,6}, /* KEY_CLEAR_HOME */
	/* KEY_RIGHT_SHIFT */
	{0xFF,5,6}, /* KEY_EQUAL */
	{0x48,6,6}, /* KEY_ARROW_U */
	{0xFF,7,6}, /* KEY_SLASH */
	{0x02,0,7}, /* KEY_1 */
	{0x4B,1,7}, /* KEY_ARROW_L */
	/* KEY_CONTROL */
	{0x03,3,7},	/* KEY_2 */
	{0x39,4,7}, /* KEY_SPACE */
	{0xFF,5,7}, /* KEY_COMMODORE */
	{0x10,6,7}, /* KEY_Q */
	{0x01,7,7}, /* KEY_RUN_STOP */
	{0x00,0,8}, /* JOY A, CIA port B: UP */
	{0x00,1,8}, /* JOY A, CIA port B: DOWN */
	{0x00,2,8}, /* JOY A, CIA port B: LEFT */
	{0x00,3,8}, /* JOY A, CIA port B: RIGHT */
	{0x00,4,8}, /* JOY A, CIA port B: FIRE */
	{0x00,8,0}, /* JOY B, CIA port A: UP */
	{0x00,8,1}, /* JOY B, CIA port A: DOWN */
	{0x00,8,2}, /* JOY B, CIA port A: LEFT */
	{0x00,8,3}, /* JOY B, CIA port A: RIGHT */
	{0x00,8,4}, /* JOY B, CIA port A: FIRE */
};

static uint8_t keys_press_cnt = 0;
key_t array_keys[128] = { 0 };


void keybd_cb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	uint8_t i = 0;
	uint8_t key_cnt = 0;

	if(action == GLFW_PRESS)
	{
		for(i = 0; i < MAX_SIMUL_KEYS; i++)
		{
			if(array_keys[i].scancode == 0)
			{
				array_keys[i].scancode = scancode;
				array_keys[i].key = key;
				break;
			}
		}
	}
	else if(action == GLFW_RELEASE)
	{
		for(i = 0; i < MAX_SIMUL_KEYS; i++)
		{
			/* For some reason we do not get scancode when key is released.. */
			if(array_keys[i].key == key)
			{
				array_keys[i].scancode = 0;
				array_keys[i].key = 0;
				break;
			}
		}
	}

	/* System */
	if (key == GLFW_KEY_F12 && action == GLFW_PRESS)
	{
		main_open_file();
	}

	/* Joystick */
	if(key == GLFW_KEY_KP_8 ||
	   key == GLFW_KEY_KP_6 ||
	   key == GLFW_KEY_KP_4 ||
	   key == GLFW_KEY_KP_2 ||
	   key == GLFW_KEY_KP_0 ||
	   key == GLFW_KEY_UP ||
	   key == GLFW_KEY_RIGHT ||
	   key == GLFW_KEY_LEFT  ||
	   key == GLFW_KEY_DOWN ||
	   key == GLFW_KEY_RIGHT_CONTROL)
	{
		if(key == GLFW_KEY_KP_8) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_A, IF_JOYST_ACTION_UP, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_KP_6) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_A, IF_JOYST_ACTION_RIGHT, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_KP_4) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_A, IF_JOYST_ACTION_LEFT, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_KP_2) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_A, IF_JOYST_ACTION_DOWN, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_KP_0) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_A, IF_JOYST_ACTION_FIRE, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);

		if(key == GLFW_KEY_UP) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_B, IF_JOYST_ACTION_UP, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_RIGHT) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_B, IF_JOYST_ACTION_RIGHT, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_LEFT) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_B, IF_JOYST_ACTION_LEFT, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_DOWN) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_B, IF_JOYST_ACTION_DOWN, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
		if(key == GLFW_KEY_RIGHT_CONTROL) g_if_cc_emu.if_emu_cc_ue.ue_joyst_fp(IF_JOYST_PORT_B, IF_JOYST_ACTION_FIRE, (action == GLFW_PRESS || action == GLFW_REPEAT)? IF_JOYST_ACTION_STATE_PRESSED : IF_JOYST_ACTION_STATE_RELEASED);
	}
	/* Keyboard */
	else
	{
		uint8_t temp_array[128] = {0};
		for(i = 0; i < MAX_SIMUL_KEYS; i++)
		{
			temp_array[i] = array_keys[i].scancode & 0xFF;
		}

		g_if_cc_emu.if_emu_cc_ue.ue_keybd_fp(temp_array,
			MAX_SIMUL_KEYS,
			(mods == GLFW_MOD_SHIFT) ? IF_KEY_STATE_PRESSED : IF_KEY_STATE_RELEASED,
			(mods == GLFW_MOD_CONTROL) ? IF_KEY_STATE_PRESSED : IF_KEY_STATE_RELEASED);
	}

	/*if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);*/
}

/* Map USB keyboard scan codes */
void keybd_pop_map(uint8_t *conf_text, if_keybd_map_t *keybd_map_p)
{
	uint8_t keys_cnt = 0;
	char delimiter_p[2] = "\n";
	char scan_code_p[3];
	char matrix_x_p[2];
	char matrix_y_p[2];
	char *keys_pp[DEFAULT_KEY_MAX + 1];

	memset(keys_pp, 0x00, (DEFAULT_KEY_MAX + 1) * sizeof(char *));

	keys_pp[keys_cnt] = strtok((char *)conf_text, delimiter_p);

	/* Parse line */
	scan_code_p[0] = keys_pp[keys_cnt][0];
	scan_code_p[1] = keys_pp[keys_cnt][1];
	scan_code_p[2] = '\0';

	matrix_x_p[0] = keys_pp[keys_cnt][3];
	matrix_x_p[1] = '\0';

	matrix_y_p[0] = keys_pp[keys_cnt][5];
	matrix_y_p[1] = '\0';

	keybd_map_p[keys_cnt].scan_code = strtoul(scan_code_p, NULL, 16);
	keybd_map_p[keys_cnt].matrix_x = atoi(matrix_x_p);
	keybd_map_p[keys_cnt].matrix_y = atoi(matrix_y_p);

	keys_cnt++;

	while (keys_pp[keys_cnt - 1] != NULL)
	{
		/* Get the map string */
		keys_pp[keys_cnt] = strtok(NULL, delimiter_p);

		/* Parse line */
		scan_code_p[0] = keys_pp[keys_cnt][0];
		scan_code_p[1] = keys_pp[keys_cnt][1];
		scan_code_p[2] = '\0';

		matrix_x_p[0] = keys_pp[keys_cnt][3];
		matrix_x_p[1] = '\0';

		matrix_y_p[0] = keys_pp[keys_cnt][5];
		matrix_y_p[1] = '\0';

		keybd_map_p[keys_cnt].scan_code = strtoul(scan_code_p, NULL, 16);
		keybd_map_p[keys_cnt].matrix_x = atoi(matrix_x_p);
		keybd_map_p[keys_cnt].matrix_y = atoi(matrix_y_p);

		keys_cnt++;
	}
}

if_keybd_map_t *keybd_get_default_map()
{
	return g_default_keybd_map_p;
}



