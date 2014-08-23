#include "Pch.h"
#include "Common.h"

extern texture* g_font;

float measure_char(int c) {
	switch(c) {
		case 'I': return 4.0f;
		case 'W': return 9.0f;
		case 'M': return 9.0f;
		case ':': return 4.0f;
		case '.': return 4.0f;
		case '/': return 6.0f;
		case ' ': return 4.0f;
		case '-': return 4.0f;
		case '!': return 4.0f;
		case '(': return 5.0f;
		case ')': return 5.0f;
		case ',': return 4.0f;
		case ';': return 4.0f;
		case '\\': return 6.0f;
		case '\'': return 3.0f;
		case '"': return 5.0f;
		case '+': return 4.0f;
		case '*': return 4.0f;
		case '=': return 4.0f;
		case '<': return 6.0f;
		case '>': return 6.0f;
		case '%': return 6.0f;
	}

	return 8.0f;
}

float measure_string(const char* txt) {
	float w = 0.0f;

	for(; *txt; txt++)
		w += measure_char(toupper(*txt));

	return w;
}

void draw_char(draw_context* dc, vec2 pos, vec2 scale, int sprite, colour col) {
	int sx = sprite & 7;
	int sy = sprite / 8;

	vec2 uv0(sx / 8.0f, sy / 8.0f);
	vec2 uv1(uv0 + vec2(1.0f / 8.0f));

	vec2 fuzz(0.0001f);

	dc->rect(pos, pos + 8.0f * scale, uv0 + fuzz, uv1 - fuzz, col, col, col, col);
}

void draw_string(draw_context* dc, vec2 pos, vec2 scale, int flags, colour col, const char* txt, ...) {
	char buf[512];

	va_list ap;

	va_start(ap, txt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, txt, ap);
	va_end(ap);

	if ((flags & TEXT_CENTRE) == TEXT_CENTRE)		pos.x -= measure_string(buf) * 0.5f * scale.x;
	else if ((flags & TEXT_CENTRE)	== TEXT_RIGHT)	pos.x -= measure_string(buf) * scale.x;

	if ((flags & TEXT_VCENTRE) == TEXT_VCENTRE)	pos.y -= 3.5f * scale.y;
	else if (flags & TEXT_BOTTOM)				pos.y -= 7.0f * scale.y;

	const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:/-!@(),\\;?'\"+*=<>%&\001\002\003\004";

	dc->push_texture(g_font);

	for(const char* p = buf; *p != '\0'; p++) {
		int c = toupper(*p);
		int sprite = -1;

		if (const char* ofs = strchr(letters, c))
			sprite = (int)(intptr_t)(ofs - letters);

		if (sprite >= 0) {
			if (islower(*p))
				draw_char(dc, pos + scale * vec2(0.0f, 7.0f * 0.25f), scale * vec2(1.0f, 0.75f), sprite, col);
			else
				draw_char(dc, pos, scale, sprite, col);
		}

		pos.x += measure_char(c) * scale.x;
	}

	dc->pop_texture();
}