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
		if (player* p = overlaps_player(this)) {
			p->damage(&damage_desc(damage_type::COLLISION, 1, this));
			damage(&damage_desc(damage_type::COLLISION, 1, p));
		}
	}
}

void unit::damage(damage_desc* dd) {
	if (_flags & EF_DESTROYED)
		return;

	if ((_health -= dd->value) <= 0)
		killed();
	else
		flinch();
}

void unit::flinch() {
}

void unit::killed() {
	if (_flags & EF_ENEMY) {
		_world->kills++;
		_world->score += 5 * _world->multi;
	}

	spawn_pickup(_world, _pos);

	//SoundPlay(sound_id::DIT, 1.0f, 1.0f);

	destroy();
}