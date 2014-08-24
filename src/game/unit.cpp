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
		killed(dd);
	else
		flinch(dd);
}

void unit::flinch(damage_desc* dd) {
}

void unit::killed(damage_desc* dd) {
	if (_flags & EF_ENEMY) {
		if (dd->type == damage_type::BULLET) {
			_world->kills++;
			_world->multi++;
			//_world->score += 5 * _world->multi;

			spawn_pickup(_world, _pos);
		}
	}

	fx_explosion(_pos, 1.0f, 10, _colour, 1.0f);

	SoundPlay(sound_id::UNIT_EXPLODE, _world->r.range(0.9f, 1.0f), _world->r.range(0.85f, 1.1f));

	_world->shake += 4.0f;

	destroy();
}