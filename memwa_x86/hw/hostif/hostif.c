/*
 * memwa emulator-host interface
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


/**
 * Host interface implementation. Contains all functions necessary for
 * the emu libs to work. The interface for this file is dictated by
 * if.h file.
 */

#pragma warning(disable:4996) /* Disable warnings _CRT_SECURE_NO_WARNINGS */

#include "hostif.h"
#include "if.h"
#include "windows.h"
#include "snd.h"
#include "video.h"

uint32_t *if_host_filesys_open(char *path_p, uint8_t mode);
void if_host_filesys_close(uint32_t *fd_p);
uint32_t if_host_filesys_read(uint32_t *fd_p, uint8_t *buf_p, uint32_t length);
uint32_t if_host_filesys_write(uint32_t *fd_p, uint8_t *buf_p, uint32_t length);
void if_host_filesys_remove(char *path_p);
void if_host_filesys_flush(uint32_t *fd_p);
void if_host_filesys_seek(uint32_t *fd_p, uint32_t pos);
void if_host_sid_write(uint8_t addr, uint8_t data);
uint8_t if_host_sid_read(uint8_t addr);
uint32_t if_host_rand_get();
uint32_t if_host_time_get_ms();
void if_host_print(char *string_p, print_type_t print_type);
void if_host_stats_fps(uint8_t fps);
void if_host_stats_led(uint8_t led);
uint32_t if_host_calc_checksum(uint8_t *buffer_p, uint32_t length);
uint8_t if_host_ports_read_serial(if_emu_dev_t if_emu_dev);
void if_host_ports_write_serial(if_emu_dev_t if_emu_dev, uint8_t data);
void if_host_disp_flip(uint8_t **done_buffer_pp);
void if_host_ee_tape_play(uint8_t play);
void if_host_ee_tape_motor(uint8_t motor);

if_host_t g_if_host =
{
    {
        if_host_filesys_open,
        if_host_filesys_close,
        if_host_filesys_read,
        if_host_filesys_write,
        if_host_filesys_remove,
        if_host_filesys_flush,
        if_host_filesys_seek
    },
    {
        if_host_sid_read,
        if_host_sid_write
    },
    {
        if_host_rand_get
    },
    {
        if_host_time_get_ms
    },
    {
        if_host_print
    },
    {
        if_host_stats_fps,
        if_host_stats_led
    },
    {
        if_host_calc_checksum
    },
    {
        if_host_ports_read_serial,
        if_host_ports_write_serial,
    },
    {
        if_host_disp_flip
    },
    {
        if_host_ee_tape_play,
        if_host_ee_tape_motor
    }
};

extern if_emu_cc_t g_if_cc_emu;
extern if_emu_dd_t g_if_dd_emu;

uint32_t *if_host_filesys_open(char *path_p, uint8_t mode)
{
	return 0;
}

void if_host_filesys_close(uint32_t *fd_p)
{
	fclose((FILE *)fd_p);
	free(fd_p);
}

uint32_t if_host_filesys_read(uint32_t *fd_p, uint8_t *buf_p, uint32_t length)
{
	uint32_t bytes_read;
	char string[128];
	
	if(fd_p == NULL) return 0;

	bytes_read = fread(buf_p, 1, length, (FILE *)fd_p);

	sprintf(string, "Read %d bytes from file", bytes_read);
	if_host_print(string, PRINT_TYPE_INFO);

	return bytes_read;
}

uint32_t if_host_filesys_write(uint32_t *fd_p, uint8_t *buf_p, uint32_t length)
{
	uint32_t bytes_written;
	char string[128];

	if(fd_p == NULL) return 0;

	bytes_written = fwrite(buf_p, 1, length, (FILE *)fd_p);

	sprintf(string, "Wrote %d bytes to file", bytes_written);
	if_host_print(string, PRINT_TYPE_INFO);

	return bytes_written;
}

void if_host_filesys_remove(char *path_p)
{

}

void if_host_filesys_flush(uint32_t *fd_p)
{

}

void if_host_filesys_seek(uint32_t *fd_p, uint32_t pos)
{
	fseek((FILE *)fd_p, pos, SEEK_SET);
}

void if_host_sid_write(uint8_t addr, uint8_t data)
{
	snd_write(addr, data);
}

uint8_t if_host_sid_read(uint8_t addr)
{
	return snd_read(addr);
}

uint32_t if_host_rand_get()
{
	return 0;
}

uint32_t if_host_time_get_ms()
{
	return GetTickCount();
}

void if_host_print(char *string_p, print_type_t print_type)
{
	switch (print_type)
	{
	case PRINT_TYPE_INFO:
		printf("[INFO] %s\n", string_p);
		break;
	case PRINT_TYPE_WARNING:
		printf("[WARN] %s\n", string_p);
		break;
	case PRINT_TYPE_ERROR:
		printf("[ERR] %s\n", string_p);
		break;
	case PRINT_TYPE_DEBUG:
		printf("[DEBUG] %s\n", string_p);
		break;
	}
}

void if_host_stats_fps(uint8_t fps)
{
	char buf[28];
	sprintf(buf, "FPS: %d", fps);
	if_host_print(buf, PRINT_TYPE_INFO);
}

void if_host_stats_led(uint8_t led)
{

}

uint32_t if_host_calc_checksum(uint8_t *buffer_p, uint32_t length)
{
    uint32_t res = 0;
    uint32_t i;

    for(i = 0; i < length; i++)
    {
        res ^= buffer_p[i];
    }

    return res;
}

uint8_t if_host_ports_read_serial(if_emu_dev_t if_emu_dev)
{
    switch(if_emu_dev)
    {
        case IF_EMU_DEV_CC:
          break;
        case IF_EMU_DEV_DD:
          break;
    }

    return 0; /* Not used atm */
}

void if_host_ports_write_serial(if_emu_dev_t if_emu_dev, uint8_t data)
{
	switch (if_emu_dev)
	{
	case IF_EMU_DEV_CC: /* Computer writes to serial port */
		g_if_dd_emu.if_emu_dd_ports.if_emu_dd_ports_write_serial_fp(data);
		break;
	case IF_EMU_DEV_DD: /* Disk drive writes to serial port */
		g_if_cc_emu.if_emu_cc_ports.if_emu_cc_ports_write_serial_fp(data);
		break;
	}
}

void if_host_disp_flip(uint8_t **done_buffer_pp)
{
	video_run(*done_buffer_pp);
}

void if_host_ee_tape_play(uint8_t play)
{
    
}

void if_host_ee_tape_motor(uint8_t motor)
{

}
