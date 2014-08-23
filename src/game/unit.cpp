#include "Pch.h"
#include "Common.h"
#include "Game.h"

unit::unit(entity_type type) : entity(type), _health(1) {
	_flags |= EF_UNIT | EF_SPAWNING;
}

void unit::instant_spawn() {
	_flags &= ~EF_SPAWNING;
}

void unit::post_tick() {
	if (_flags & EF_ENEMY) {
		if (overlaps_player(this)) {
			damage();
		}
	}
}

void unit::damage() {
	if (--_health <= 0)
		killed();
	else
		flinch();
}

void unit::flinch() {
}

void unit::killed() {
	SoundPlay(sound_id::DIT, 1.0f, 1.0f);
	destroy();
}