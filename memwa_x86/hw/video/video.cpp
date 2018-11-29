/*
 * memwa video functions
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

#include "video.h"

#include <windows.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "glad.h"

#define CLUT_MAX        19
#define HIGHT			(284*2)
#define WIDTH			(400*2)

static uint32_t clut_a[CLUT_MAX] =
{
	0x000000, /* black */
	0xFFFFFF, /* white */
	0x744335, /* red */
	0x7CACBA, /* cyan */
	0x7B4890, /* violet */
	0x64974F, /* green */
	0x403285, /* blue */
	0xBFCD7A, /* yellow */
	0x7B5B2F, /* orange */
	0x4f4500, /* brown */
	0xa37265, /* light red */
	0x505050, /* grey 1 */
	0x787878, /* grey 2 */
	0xa4d78e, /* light green */
	0x786abd, /* light blue */
	0x9f9f9f, /* grey 3 */
	0x010211, /* text background */
	0x8f9fed, /* text forground */
	0x151bfb, /* marker */
};

static GLFWwindow* window;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow *video_init()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(WIDTH, HIGHT, "Memwa", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(0);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, HIGHT, 0.0);

	return window;
}

void video_run(uint8_t *frame_p)
{
	uint32_t x;
	uint32_t y;

	glBegin(GL_POINTS);
	for (x = 0; x < HIGHT; x++)
	{
		for (y = 0; y < WIDTH; y++)
		{
			uint32_t color = clut_a[frame_p[y + x * WIDTH]];

			glColor3ub((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
			glVertex2i(y, x);
		}
	}
	glEnd();

	glfwSwapBuffers(window);
}

