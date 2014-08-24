#include "Pch.h"
#include "Common.h"
#include "Game.h"

message::message() : entity(ET_MESSAGE) {
	_flags |= EF_MESSAGE;
	_colour = colours::SILVER;
	_radius = 0.5f;
	_time = 45;
}

void message::tick() {
	if (--_time <= 0)
		destroy();
}

void message::draw(draw_context* dc) {
	float f = clamp(_time / 15.0f, 0.0f, 1.0f);

	draw_string(dc, vec2(_radius + 4.0f, 0.0f), _radius * f, TEXT_CENTRE | TEXT_VCENTRE, colours::ORANGE * f, "%s", _text);
}

void spawn_message(world* w, vec2 pos, bool important, const char* txt, ...) {
	message* ent = spawn_entity(w, new message);

	va_list ap;

	va_start(ap, txt);
	_vsnprintf_s(ent->_text, sizeof(ent->_text), _TRUNCATE, txt, ap);
	va_end(ap);

	ent->_pos = pos;

	if (important) {
		ent->_radius = 0.75f;
		ent->_time = 60;
	}
}