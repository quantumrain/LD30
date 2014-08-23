#include "Pch.h"
#include "Common.h"
#include "Game.h"

unit::unit(entity_type type) : entity(type) {
	_flags |= EF_UNIT | EF_SPAWNING;
}

void unit::instant_spawn() {
	_flags &= ~EF_SPAWNING;
}