#include "draw.h"
#include "sdk.h"

c_draw draw;
cscreensize_t screen_size;

void c_draw::initialize_font()
{
	font = ints.isurface->create_font();
	ints.isurface->set_font(font, "Courier New", 15, 0, 0, 0, 0x200);
	ints.isurface->get_screen_size(screen_size.screen_width, screen_size.screen_height);
}

void c_draw::draw_string(int x, int y, DWORD clr, const char * text, ...)
{
	if (text == NULL) return;

	va_list va_alist;
	char szBuffer[1024] = {'\0'};
	wchar_t szString[1024] = {'\0'};

	int width, height;

	va_start(va_alist, text);
	vsprintf_s(szBuffer, text, va_alist);
	va_end(va_alist);

	MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, szString, 128);

	ints.isurface->draw_set_text_pos(x, y);
	ints.isurface->draw_set_text_font(font);
	ints.isurface->draw_set_text_color(RED(clr), GREEN(clr), BLUE(clr), ALPHA(clr));
	ints.isurface->draw_print_text(szString, wcslen(szString));
}

bool c_draw::w2s(vector & origin, vector & scr)
{
	const matrix3x4 & world_to_screen = ints.engine->w2s_matrix();

	float w = world_to_screen[3][0] * origin[0] + world_to_screen[3][1] * origin[1] + world_to_screen[3][2] * origin[2]
		+ world_to_screen[3][3];
	scr.z = 0;

	if (w > 0.001)
	{
		float fl1DBw = 1 / w;
		scr.x = (screen_size.screen_width / 2) + (0.5 * ((world_to_screen[0][0] * origin[0] + world_to_screen[0][1] *
				origin[1] + world_to_screen[0][2] * origin[2] + world_to_screen[0][3]) * fl1DBw) * screen_size.
			screen_width
			+ 0.5);
		scr.y = (screen_size.screen_height / 2) - (0.5 * ((world_to_screen[1][0] * origin[0] + world_to_screen[1][1] *
				origin[1] + world_to_screen[1][2] * origin[2] + world_to_screen[1][3]) * fl1DBw) * screen_size.
			screen_height
			+ 0.5);
		return true;
	}

	return false;
}
