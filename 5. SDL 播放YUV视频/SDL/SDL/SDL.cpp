// SDL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

extern "C" {
#include "SDL.h"
}

//Bit per Pixel
const int bpp = 12;

int screen_w = 500, screen_h = 500;

// 根据不同的YUV视频，来设置不同的 宽 * 高 数据 
const int pixel_w = 352, pixel_h = 288;

unsigned char buffer[pixel_w*pixel_h*bpp / 8];

//Refresh Event
#define REFRESH_EVENT  (SDL_USEREVENT + 1)

int thread_exit = 0;

int refresh_video(void *opaque) {
	while (thread_exit == 0) {
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	SDL_Window *window;

	// 创建SDL窗口
	window = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	
	// 判断是否创建窗口成功
	if (!window) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}

	// 创建SDL渲染器
	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, 0);

	// 声明像素格式
	Uint32 pixformat = 0;

	// IYUV: Y + U + V  (3 planes)
	// YV12: Y + V + U  (3 planes)
	// I420也叫IYUV, 也叫YUV420
	pixformat = SDL_PIXELFORMAT_IYUV;

	// 按照YUV视频的宽高创建SDL纹理对象
	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

	FILE *fp = NULL;

	fp = fopen("111.yuv", "rb+");
	if (fp == NULL) {
		printf("cannot open this file\n");
		return -1;
	}

	SDL_Rect sdlRect;

	SDL_Thread *refresh_thread = SDL_CreateThread(refresh_video, NULL, NULL);
	SDL_Event event;
	while (1) {
		//Wait
		SDL_WaitEvent(&event);
		if (event.type == REFRESH_EVENT) {
			// 读取一帧数数据到缓冲区
			if (fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp) != pixel_w * pixel_h*bpp / 8) {
				// Loop
				fseek(fp, 0, SEEK_SET);
				fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp);
			}
			// 将数据更新到纹理
			SDL_UpdateTexture(sdlTexture, NULL, buffer, pixel_w);
			//FIX: If window is resize
			sdlRect.x = 0;
			sdlRect.y = 0;
			sdlRect.w = screen_w;
			sdlRect.h = screen_h;

			SDL_RenderClear(sdlRenderer);
			// 将更新后的纹理拷贝到渲染器
			SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
			// 渲染器显示画面
			SDL_RenderPresent(sdlRenderer);
			//Delay 40ms -- Dealy时常根据帧率进行调整
			SDL_Delay(40);
		}
		else if (event.type == SDL_WINDOWEVENT) {
			//If Resize
			SDL_GetWindowSize(window, &screen_w, &screen_h);
		}
		else if (event.type == SDL_QUIT) {
			break;
		}
	}
	return 0;
}
