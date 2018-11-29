/*
 * memwa sound functions
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
#include "snd.h"
#include "sdl.h"
#include "sid.h"

#define SAMPLES   4096
#define BANKS     8
#define FREQ      44100
#define CPU_FREQ  985248

struct snd_t {
	SID_ *mysid;                  // input device
	SDL_AudioSpec desired;        // desired parameter for output device
	SDL_AudioSpec obtained;       // obtained parameter for output device
	short buf[BANKS][SAMPLES]; // input buffer
	uint16_t sample_input_counter; // input sample counter
	uint16_t sample_output_counter; // input sample counter
	uint8_t sample_bank_input;      // current input bank
	uint8_t sample_bank_output;     // current output bank
} snd;

static void calc_buffer(void *userdata, uint8_t *buf, int count)
{
	uint16_t i = 0;
	struct snd_t *mysnd = (struct snd_t *)userdata;

	SDL_LockAudio();

	for (i = 0; i < count; i += 2)
	{
		mysnd->buf[snd.sample_bank_output][snd.sample_output_counter] = (short)(mysnd->buf[snd.sample_bank_output][snd.sample_output_counter] * 0.005);
		*(buf + i) = mysnd->buf[snd.sample_bank_output][snd.sample_output_counter] >> 8;
		*(buf + i + 1) = mysnd->buf[snd.sample_bank_output][snd.sample_output_counter] & 0xFF;
		snd.sample_output_counter++;
	}

	snd.sample_bank_output++;
	snd.sample_output_counter = 0;

	if (snd.sample_bank_output >= BANKS)
		snd.sample_bank_output = 0;

	SDL_UnlockAudio();
}

void snd_init()
{
	memset(&snd, 0, sizeof(snd));

	// Open audio device
	snd.desired.callback = calc_buffer;
	snd.desired.userdata = &snd;
	snd.desired.freq = FREQ;
	snd.desired.format = AUDIO_U16SYS;
	snd.desired.channels = 1;
	snd.desired.samples = SAMPLES;

	if (SDL_OpenAudio(&snd.desired, &snd.obtained) < 0)
	{
		fprintf(stderr, "Couldn't initialize audio (%s)\n", SDL_GetError());
		exit(1);
	}

	snd.mysid = new SID_();

	snd.mysid->enable_filter(false);
	snd.mysid->enable_external_filter(false);
	snd.mysid->set_chip_model(MOS8580); // MOS6581

	snd.mysid->set_sampling_parameters(CPU_FREQ, SAMPLE_FAST, FREQ);
}

void snd_write(uint8_t addr, uint8_t data)
{
	snd.mysid->write((reg8)addr, (reg8)data);
}

uint8_t snd_read(uint8_t addr)
{
	return snd.mysid->read((reg8)addr);
}

void snd_run(uint32_t cc)
{
	int delta_t = cc;
	static uint8_t started = 0;

	while (delta_t) {
		snd.sample_input_counter += snd.mysid->clock(delta_t, &snd.buf[snd.sample_bank_input][snd.sample_input_counter], SAMPLES - snd.sample_input_counter);
		if ((SAMPLES - snd.sample_input_counter) <= 0) break;
	}

	if (snd.sample_input_counter >= SAMPLES)
	{
		snd.sample_bank_input++;
		if (snd.sample_bank_input >= BANKS)
			snd.sample_bank_input = 0;

		snd.sample_input_counter = 0;

		if (snd.sample_bank_input == BANKS / 2)
		{
			SDL_PauseAudio(0);
			started = 1;
		}

		if (started == 1)
		{
			do {
				;
			} while (snd.sample_output_counter <= SAMPLES / 2);
		}
	}
}
