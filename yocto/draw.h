#pragma once

#include "sdk.h"

// darkstorm

#define RED(COLORCODE)	((int) ( COLORCODE >> 24) )
#define BLUE(COLORCODE)	((int) ( COLORCODE >> 8 ) & 0xFF )
#define GREEN(COLORCODE)	((int) ( COLORCODE >> 16 ) & 0xFF )
#define ALPHA(COLORCODE)	((int) COLORCODE & 0xFF )
#define COLORCODE(r,g,b,a)((DWORD)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))

class c_draw
{
private:
	unsigned long font;
public:
	void initialize_font();
	void draw_string(int x, int y, DWORD dwColor, const char * pszText, ...);
	bool w2s(vector & vOrigin, vector & vScreen);
};

extern c_draw draw;
