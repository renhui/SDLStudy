// SDL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

extern "C" {
#include "SDL.h"
}

/**
 *
 * 使用SDL2播放PCM音频采样数据。SDL实际上是对底层绘图API（Direct3D，OpenGL）的封装，使用起来明显简单于直接调用底层API。
 *
 * 函数调用步骤如下:
 *
 * [初始化]
 * SDL_Init(): 初始化SDL。
 * SDL_OpenAudio(): 根据参数（存储于SDL_AudioSpec）打开音频设备。
 * SDL_PauseAudio(): 播放音频数据。
 *
 * [循环播放数据]
 * SDL_Delay(): 延时等待播放完成。
 * 
 * [播放音频的基本原则]
 * 声卡向你要数据而不是你主动推给声卡
 * 数据的多少是由音频参数决定的
 */

//Buffer:
//|-----------|-------------|
//chunk-------pos---len-----|

static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;

void  fill_audio(void *udata, Uint8 *stream, int len) {
	//SDL 2.0
	SDL_memset(stream, 0, len);
	if (audio_len == 0)
		return;
	len = (len > audio_len ? audio_len : len);

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}

int main(int argc, char* argv[])
{
	//Init
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	//SDL_AudioSpec
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = 48000;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = 2;
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024;
	wanted_spec.callback = fill_audio;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
		printf("can't open audio.\n");
		return -1;
	}

	FILE *fp = fopen("test.pcm", "rb+");

	if (fp == NULL) {
		printf("cannot open this file\n");
		return -1;
	}
	int pcm_buffer_size = 4096;
	char *pcm_buffer = (char *)malloc(pcm_buffer_size);
	int data_count = 0;

	//Play
	SDL_PauseAudio(0);

	while (1) {
		if (fread(pcm_buffer, 1, pcm_buffer_size, fp) != pcm_buffer_size) {
			// Loop
			fseek(fp, 0, SEEK_SET);
			fread(pcm_buffer, 1, pcm_buffer_size, fp);
			data_count = 0;
		}
		printf("Now Playing %10d Bytes data.\n", data_count);
		data_count += pcm_buffer_size;
		//Set audio buffer (PCM data)
		audio_chunk = (Uint8 *)pcm_buffer;
		//Audio buffer length
		audio_len = pcm_buffer_size;
		audio_pos = audio_chunk;

		while (audio_len > 0)//Wait until finish
			SDL_Delay(1);
	}
	free(pcm_buffer);
	SDL_Quit();
	return 0;
}
