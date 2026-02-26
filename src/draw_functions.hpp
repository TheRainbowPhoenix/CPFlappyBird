// draw_functions.hpp by InterChan
// minor changes by s3ansh33p for new file structure

#pragma once

// change the last folder name below to your game folder name in the converted "textures" folder
#define PATH_PREFIX "\\fls0\\usr\\textures\\CPFlappyBird\\"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "shaders.hpp"
#include "calc.hpp"

#define LOAD_TEXTURE_PTR(path, pointer) uint16_t *pointer = load_texture(path)
#define LOAD_FONT_PTR(path, pointer) uint8_t *pointer = load_font(path)
#define DRAW_TEXTURE(texturepointer, x, y) draw_texture_shader(texturepointer, x, y, 1, 0)
#define DRAW_TEXTURE_FRAME(texturepointer, x, y, frame) draw_texture_shader(texturepointer, x, y, 2, frame)
#define DRAW_FONT(fontpointer, text, x, y, color, wrapLength) draw_font_shader(fontpointer, text, x, y, color, wrapLength, 1, 0, 0)

// start with zero assets loaded
uint16_t memUsed = 0;
uint16_t txLoaded = 0; //textures loaded
uint16_t fLoaded = 0; //fonts loaded

inline uint16_t uint8to16(uint8_t highByte, uint8_t lowByte) {
	return (highByte << 8) | lowByte;
}

uint16_t *load_texture(const char *texturepath) {
	char concatpath[128];
	#ifdef PATH_PREFIX
		strcpy(concatpath, PATH_PREFIX);
	#else
		strcpy(concatpath, "\\fls0\\usr\\textures\\"); // try not to do this, it's a bad idea
	#endif
	strcat(concatpath, texturepath);
	FILE *fd = fopen(concatpath, "rb");
	if (fd) {
		uint16_t info[2];
		fread(info, 1, 4, fd);
		uint16_t w = info[0];
		uint16_t h = info[1];
		uint16_t *result = (uint16_t*)malloc(w*h*2+4);
		memUsed += (w*h*2)+4;
		txLoaded += 1;
		fseek(fd, 0, SEEK_SET);
		fread(result, 1, w*h*2+4, fd);
		fclose(fd);
		return result;
	}
	return 0;
}

void draw_texture_shader(uint16_t *texturepointer, int16_t x, int16_t y, uint16_t shaderID, int shaderArg) {
	uint16_t w = texturepointer[0];
	uint16_t h = texturepointer[1];
	int k = 2;
	for (int16_t j = 0; j < h; j++) {
		for (int16_t i = 0; i < w; i++) {
			shader(x, y, w, h, i, j, texturepointer[k], shaderID, shaderArg);
			k++;
		}
	}
}

uint8_t *load_font(const char *fontpath) {
	char concatpath[128];
	#ifdef FONT_PREFIX
		strcpy(concatpath, FONT_PREFIX);
	#else
		strcpy(concatpath, "\\fls0\\usr\\fonts\\");
	#endif
	strcat(concatpath, fontpath);
	FILE *fd = fopen(concatpath, "rb");
	if (fd) {
		uint16_t info[4];
		fread(info, 1, 4, fd);
		uint16_t w = info[0];
		uint16_t h = info[1];
		uint8_t *result = (uint8_t*)malloc(95*w*h/8+5);
		memUsed += (95*w*h/8)+5;
		fLoaded += 1;
		fseek(fd, 0, SEEK_SET);
		fread(result, 1, (95*w*h/8)+5, fd);
		fclose(fd);
		return result;
	}
	return 0;
}

#define CHAR_SPACING 1
void draw_font_shader(uint8_t *fontpointer, const char *text, int16_t x, int16_t y, uint16_t color, uint16_t wrapLength, int16_t lineSpacing, uint16_t shaderID, int shaderArg) {
	uint16_t w = uint8to16(fontpointer[0], fontpointer[1]);
	uint16_t h = uint8to16(fontpointer[2], fontpointer[3]);
	uint16_t newlines = 0;
	uint16_t chars_since_newline = 0;
	for (uint16_t textchar = 0; textchar < strlen(text); textchar++) { // repeat for each character in text
		if (text[textchar] == 10) {
			newlines++;
			chars_since_newline = 0;
		} else if (text[textchar] >= 32 && text[textchar] <= 126) {
			if (wrapLength > 0 && (w+CHAR_SPACING)*(chars_since_newline+1) > wrapLength) {
				newlines++;
				chars_since_newline = 0;
			}
			int current_byte = (text[textchar]-32)*w*h/8 + 4;
			uint8_t current_bit = 128 >> (((text[textchar]-32)*w*h)%8);
			for (int charbit = 0; charbit < w*h; charbit++) { // read bits of the target font character and draw pixels
				if (fontpointer[current_byte] & current_bit) {
					shader(x, y, w, h, charbit % w + (w+CHAR_SPACING)*chars_since_newline, charbit / w + (h+lineSpacing)*newlines, color, shaderID, shaderArg);
				}
				current_bit >>= 1;
				if (current_bit < 1) {
					current_bit = 128;
					current_byte++;
				}
			}
			chars_since_newline++;
		}

	}
}
