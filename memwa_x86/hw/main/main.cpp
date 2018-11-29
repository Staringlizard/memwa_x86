/*
 * memwa main file
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
#include <shobjidl.h>
#include "glfw3.h"
#include "sid.h"
#include "snd.h"
#include "main.h"
#include "video.h"
#include "keybd.h"

extern "C"
{	
	#include "if.h"
	#include "romcc.h"
	#include "romdd.h"
}

#pragma warning(disable:4996) /* Disable warnings _CRT_SECURE_NO_WARNINGS */

#define MAX_EXEC_CYCLES 256


static uint8_t *disp_buffer1;
static uint8_t *disp_buffer2;
static uint8_t *disp_buffer3;
static uint8_t *ram_mem_p;
static uint8_t *rom_mem_p;
static uint8_t *io_mem_p;
static uint8_t *util_mem1_p;
static uint8_t *util_mem2_p;
static uint8_t *sprite_mem1_p;
static uint8_t *sprite_mem2_p;
static uint8_t *sprite_mem3_p;
static uint8_t array_key[10];
static if_keybd_map_t g_keybd_map_a[DEFAULT_KEY_MAX + 1];

static uint8_t *ram_rom_mem_dd_p;
static uint8_t *util_mem1_dd_p;
static uint8_t *util_mem2_dd_p;

static FILE *fd_p = NULL;
static uint32_t local_errno = 0;

extern "C" if_emu_cc_t g_if_cc_emu;
extern "C" if_emu_dd_t g_if_dd_emu;

static GLFWwindow* window;

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv)
{
	disp_buffer1 = (uint8_t *)calloc(IF_MEMORY_CC_SCREEN_BUFFER1_SIZE, 1);
	disp_buffer2 = (uint8_t *)calloc(IF_MEMORY_CC_SCREEN_BUFFER2_SIZE, 1);
	disp_buffer3 = (uint8_t *)calloc(IF_MEMORY_CC_SCREEN_BUFFER3_SIZE, 1);

	ram_mem_p = (uint8_t *)VirtualAlloc(NULL, 0x10000,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	rom_mem_p = (uint8_t *)VirtualAlloc(NULL, 0x10000,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	ram_rom_mem_dd_p = (uint8_t *)VirtualAlloc(NULL, 0x10000,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	if (((unsigned int)ram_mem_p & 0xFFFF) != 0)
	{
		printf("C64 ram mem failed to align to 0x10000");
		exit(1);
	}
	else if (((unsigned int)rom_mem_p & 0xFFFF) != 0)
	{
		printf("C64 rom mem failed to align to 0x10000");
		exit(1);
	}

	/* Audio init */
	snd_init();

	/* Emulator init */
	io_mem_p = (uint8_t *)calloc(IF_MEMORY_CC_IO_SIZE, 1);
	util_mem1_p = (uint8_t *)calloc(IF_MEMORY_CC_UTIL1_SIZE, 1);
	util_mem2_p = (uint8_t *)calloc(IF_MEMORY_CC_UTIL2_SIZE, 1);
	sprite_mem1_p = (uint8_t *)calloc(IF_MEMORY_CC_SPRITE1_SIZE, 1);
	sprite_mem2_p = (uint8_t *)calloc(IF_MEMORY_CC_SPRITE2_SIZE, 1);
	sprite_mem3_p = (uint8_t *)calloc(IF_MEMORY_CC_SPRITE3_SIZE, 1);

	util_mem1_dd_p = (uint8_t *)calloc(IF_MEMORY_DD_UTIL1_SIZE, 1);
	util_mem2_dd_p = (uint8_t *)calloc(IF_MEMORY_DD_UTIL2_SIZE, 1);

	memcpy((uint8_t *)(rom_mem_p + CC_BROM_LOAD_ADDR), rom_cc_get_memory(ROM_CC_SECTION_BROM), IF_MEMORY_CC_BROM_ACTUAL_SIZE);
	memcpy((uint8_t *)(rom_mem_p + CC_CROM_LOAD_ADDR), rom_cc_get_memory(ROM_CC_SECTION_CROM), IF_MEMORY_CC_CROM_ACTUAL_SIZE);
	memcpy((uint8_t *)(rom_mem_p + CC_KROM_LOAD_ADDR), rom_cc_get_memory(ROM_CC_SECTION_KROM), IF_MEMORY_CC_KROM_ACTUAL_SIZE);

	memcpy((uint8_t *)(ram_rom_mem_dd_p + DD_DOS_LOAD_ADDR), rom_dd_get_memory(ROM_DD_SECTION_DOS), IF_MEMORY_DD_DOS_ACTUAL_SIZE);

	memcpy((uint8_t *)g_keybd_map_a, keybd_get_default_map(), DEFAULT_KEY_MAX * sizeof(if_keybd_map_t));

	/* Give commodore computer (cc) some memory to work with */
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)ram_mem_p, IF_MEM_CC_TYPE_RAM);
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)rom_mem_p, IF_MEM_CC_TYPE_BROM);
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)rom_mem_p, IF_MEM_CC_TYPE_CROM);
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)rom_mem_p, IF_MEM_CC_TYPE_KROM);
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)io_mem_p, IF_MEM_CC_TYPE_IO);
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)util_mem1_p, IF_MEM_CC_TYPE_UTIL1); /* subscription read storage by emu */
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)util_mem2_p, IF_MEM_CC_TYPE_UTIL2); /* subscription write storage by emu */
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)sprite_mem1_p, IF_MEM_CC_TYPE_SPRITE1); /* sprite virtual layer (background) by emu */
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)sprite_mem2_p, IF_MEM_CC_TYPE_SPRITE2); /* sprite virtual layer (forground) by emu */
	g_if_cc_emu.if_emu_cc_mem.mem_set_fp((uint8_t *)sprite_mem3_p, IF_MEM_CC_TYPE_SPRITE3); /* sprite mapping by emu */

	/* Give disk drive (dd) some memory to work with */
	g_if_dd_emu.if_emu_dd_mem.mem_set_fp((uint8_t *)ram_rom_mem_dd_p, IF_MEM_DD_TYPE_ALL);
	g_if_dd_emu.if_emu_dd_mem.mem_set_fp((uint8_t *)util_mem1_dd_p, IF_MEM_DD_TYPE_UTIL1);
	g_if_dd_emu.if_emu_dd_mem.mem_set_fp((uint8_t *)util_mem2_dd_p, IF_MEM_DD_TYPE_UTIL2);

	g_if_cc_emu.if_emu_cc_display.display_layer_set_fp((uint8_t *)disp_buffer2);
	g_if_cc_emu.if_emu_cc_ue.ue_keybd_map_set_fp(g_keybd_map_a);
	g_if_cc_emu.if_emu_cc_op.op_init_fp();
	g_if_dd_emu.if_emu_dd_op.op_init_fp();

	g_if_cc_emu.if_emu_cc_display.display_lock_frame_rate_fp(0); /* Getting too low resolution for timer to sync to 50fps in here */
	g_if_cc_emu.if_emu_cc_display.display_limit_frame_rate_fp(0);

	/* Video init */
	window = video_init();

	/* Key callback */
	glfwSetKeyCallback(window, keybd_cb);

	while (!glfwWindowShouldClose(window))
	{
		uint32_t i;
		for (i = 0; i < MAX_EXEC_CYCLES; i += 1)
		{
			g_if_cc_emu.if_emu_cc_op.op_run_fp(1);
			g_if_dd_emu.if_emu_dd_op.op_run_fp(1);
		}
		glfwPollEvents();
		snd_run(MAX_EXEC_CYCLES);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void main_open_file()
{
	char filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = "Text Files\0*.d64\0Any File\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a File!";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
		printf("You chose the file %s", filename);
	}

	fd_p = fopen(filename, "rb");
	local_errno = errno;


	if (fd_p != NULL)
	{
		g_if_dd_emu.if_emu_dd_disk_drive.disk_drive_load_fp((uint32_t *)fd_p);
	}
}
