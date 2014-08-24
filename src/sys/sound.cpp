#include "pch.h"
#include "Common.h"

#pragma comment(lib, "xaudio2.lib")

struct SoundVariant {
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;
	array<IXAudio2SourceVoice*> voices;
	uint8_t* data;

	SoundVariant() : wfx(), buffer(), data() { }
	~SoundVariant() { delete [] data; }
};

struct Sound {
	list<SoundVariant> variants;
	int cooldown_frames;
	int max_cooldown_frames;

	Sound() : cooldown_frames(), max_cooldown_frames(2) { }
};

IXAudio2* gXAudio;
Sound gSound[sound_id::MAX];

// riff loader

#define RIFF_FOURCC_RIFF	MAKEFOURCC('R', 'I', 'F', 'F')
#define RIFF_FOURCC_DATA	MAKEFOURCC('d', 'a', 't', 'a')
#define RIFF_FOURCC_FMT		MAKEFOURCC('f', 'm', 't', ' ')
#define RIFF_FOURCC_WAVE	MAKEFOURCC('W', 'A', 'V', 'E')

struct riff_chunk {
	const uint8_t* data;
	uint32_t size;

	riff_chunk() : data(), size() { }
	riff_chunk(const uint8_t* data_, uint32_t size_) : data(data_), size(size_) { }
	operator bool() const { return data != 0; }
};

riff_chunk riff_find_chunk(const uint8_t* riff_data, uint32_t riff_size, DWORD fourcc) {
	uint32_t offset = 4;

	while(offset < riff_size) {
		uint32_t code = *(const uint32_t*)(riff_data + offset);
		uint32_t size = *(const uint32_t*)(riff_data + offset + 4);

		if (code == fourcc)
			return riff_chunk(riff_data + offset + 8, size);

		offset += 8 + size;
	}

	return riff_chunk();
}

bool load_riff(IXAudio2* xAudio, Sound* sound, const char* path, int max_voices) {
	file_buf fb;

	if (!load_file(&fb, path))
		return false;

	uint32_t* p = (uint32_t*)fb.data;

	if (fb.size <= 12 || p[0] != RIFF_FOURCC_RIFF || p[2] != RIFF_FOURCC_WAVE)
		return false;

	uint32_t riff_size = p[1];

	if (riff_size > ((uint32_t)fb.size - 8))
		return false;

	riff_chunk ch_fmt = riff_find_chunk(fb.data + 8, riff_size, RIFF_FOURCC_FMT);
	riff_chunk ch_data = riff_find_chunk(fb.data + 8, riff_size, RIFF_FOURCC_DATA);

	if (!ch_fmt || !ch_data || ch_fmt.size < 16 || ch_data.size > riff_size)
		return false;

	SoundVariant* sv = new SoundVariant;

	if ((sv->data = new uint8_t[ch_data.size]) == 0)
		return false;

	memcpy(&sv->wfx, ch_fmt.data, min((uint32_t)sizeof(sv->wfx), ch_fmt.size));
	memcpy(sv->data, ch_data.data, ch_data.size);

	sv->buffer.AudioBytes	= ch_data.size;
	sv->buffer.pAudioData	= sv->data;
	sv->buffer.Flags			= XAUDIO2_END_OF_STREAM;

	for(int i = 0; i < max_voices; i++) {
		if (auto v = sv->voices.push_back(0)) {
			if (FAILED(xAudio->CreateSourceVoice(v, (WAVEFORMATEX*)&sv->wfx, 0, XAUDIO2_MAX_FREQ_RATIO))) {
				delete [] sv->data;
				sv->data = 0;
				delete sv;
				return false;
			}
		}
	}

	sound->variants.push_back(sv);

	return true;
}

// sound sys

void SoundInit() {
	if (FAILED(XAudio2Create(&gXAudio, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		debug("XAudioCreate failed");
		return;
	}

	IXAudio2MasteringVoice* masterVoice = 0;

	if (FAILED(gXAudio->CreateMasteringVoice(&masterVoice))) {
		debug("CreateMasterVoice failed");
		return;
	}

	load_riff(gXAudio, &gSound[(int)sound_id::DIT],				"data\\dit.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::PICKUP_COLLECT],	"data\\pickup_collect.wav", 6);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_FIRE],		"data\\player_fire.wav", 5);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_FIRE],		"data\\player_fire_1.wav", 5);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_FIRE],		"data\\player_fire_2.wav", 5);
	load_riff(gXAudio, &gSound[(int)sound_id::UNIT_EXPLODE],	"data\\unit_explode.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::UNIT_EXPLODE],	"data\\unit_explode_1.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::UNIT_EXPLODE],	"data\\unit_explode_2.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::UNIT_EXPLODE],	"data\\unit_explode_3.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::UNIT_EXPLODE],	"data\\unit_explode_3.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::BULLET_HIT_WALL],	"data\\bullet_hit_wall.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::BULLET_DEFLECT],	"data\\bullet_deflect.wav", 1);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_HIT],		"data\\player_hit.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_DIE],		"data\\player_die.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_RECHARGE],	"data\\player_recharge.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::TRACKER_SPAWN],	"data\\tracker_spawn.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::SHOOTING_STAR_HIT],"data\\shooting_star_hit.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_SPAWN],	"data\\player_spawn.wav", 1);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_SPAWN_END],"data\\player_spawn_end.wav", 1);
	load_riff(gXAudio, &gSound[(int)sound_id::PLAYER_SHIELD_END],"data\\player_shield_end.wav", 1);
	load_riff(gXAudio, &gSound[(int)sound_id::ASTEROID_BOUNCE],	"data\\asteroid_bounce.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::TRACKER_BOUNCE],	"data\\tracker_bounce.wav", 3);
	load_riff(gXAudio, &gSound[(int)sound_id::SHOOTING_STAR_SPAWN],"data\\shooting_star_spawn.wav", 8);
	load_riff(gXAudio, &gSound[(int)sound_id::SHOOTING_STAR_SPAWN_END],"data\\shooting_star_spawn_end.wav", 8);
	load_riff(gXAudio, &gSound[(int)sound_id::HISCORE_BEAT],	"data\\hiscore_beat.wav", 8);

	gSound[(int)sound_id::PICKUP_COLLECT].max_cooldown_frames = 0;
	gSound[(int)sound_id::BULLET_DEFLECT].max_cooldown_frames = 5;
	gSound[(int)sound_id::SHOOTING_STAR_HIT].max_cooldown_frames = 5;
	gSound[(int)sound_id::ASTEROID_BOUNCE].max_cooldown_frames = 15;
	gSound[(int)sound_id::TRACKER_BOUNCE].max_cooldown_frames = 5;
	gSound[(int)sound_id::SHOOTING_STAR_SPAWN].max_cooldown_frames = 5;
	gSound[(int)sound_id::SHOOTING_STAR_SPAWN_END].max_cooldown_frames = 5;
}

void SoundUpdate() {
	for(int i = 0; i < (int)sound_id::MAX; i++) {
		Sound& sound = gSound[i];

		if (sound.cooldown_frames > 0)
			sound.cooldown_frames--;
	}
}

void SoundShutdown()  {
	if (!gXAudio)
		return;

	for(int i = 0; i < (int)sound_id::MAX; i++) {
		Sound& sound = gSound[i];

		for(auto& sv : sound.variants) {
			for(auto& v : sv->voices) {
				if (v) {
					v->Stop();
					v->FlushSourceBuffers();
					v->DestroyVoice();
					v = 0;
				}
			}

			delete [] sv->data;
			sv->data = 0;
		}
	}

	gXAudio->Release();
}

random g_sound_rand;

bool SoundPlay(sound_id sid, float freq, float volume) {
	if (!gXAudio)
		return false;

	Sound& sound = gSound[(int)sid];

	if (sound.cooldown_frames > 0)
		return false;

	if (sound.variants.empty())
		return false;

	SoundVariant* sv = sound.variants[g_sound_rand.rand(sound.variants.size())];

	IXAudio2SourceVoice* best = 0;
	UINT64 bestSamples = 0;

	for(auto v : sv->voices) {
		if (v) {
			XAUDIO2_VOICE_STATE s;
			v->GetState(&s);

			if (s.BuffersQueued == 0) {
				best = v;
				break;
			}

			if (bestSamples < s.SamplesPlayed) {
				best = v;
				bestSamples = s.SamplesPlayed;
			}
		}
	}

	if (!best)
		return false;

	// todo: should store all sounds played this frame and coalesce similar ones
	best->Stop();
	best->FlushSourceBuffers();
	best->SubmitSourceBuffer(&sv->buffer);
	best->SetFrequencyRatio(freq);
	best->SetVolume(volume);
	best->Start();

	sound.cooldown_frames = sound.max_cooldown_frames;

	return true;
}