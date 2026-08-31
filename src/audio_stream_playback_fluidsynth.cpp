#include "audio_stream_playback_fluidsynth.h"

using namespace godot;

AudioStreamPlaybackFluidSynth::AudioStreamPlaybackFluidSynth() : player(nullptr), buffer(nullptr), active(false) {
    buffer = new float[44100 * 2];
}

AudioStreamPlaybackFluidSynth::~AudioStreamPlaybackFluidSynth() {
    if (player != nullptr) {
        fluid_player_stop(player);
        delete_fluid_player(player);
    }
    delete[] buffer;
}

void AudioStreamPlaybackFluidSynth::_start(double p_from_pos) {
    active = true;
    player = nullptr;

    if (base.is_null() || base->synth == nullptr) {
        return;
    }

    // Without a MIDI file this is a live-only session: the stream keeps
    // rendering the synth's current state until stopped.
    if (base->midi_file.is_null()) {
        return;
    }

    PackedByteArray bytes = base->midi_file->get_data();
    if (bytes.size() <= 0) {
        return;
    }

    fluid_player_t *p = new_fluid_player(base->synth);
    if (p == nullptr) {
        return;
    }
    if (fluid_player_add_mem(p, bytes.ptr(), bytes.size()) == FLUID_FAILED) {
        delete_fluid_player(p);
        return;
    }
    fluid_player_play(p);
    player = p;
}

void AudioStreamPlaybackFluidSynth::_stop() {
    active = false;
    if (player != nullptr) {
        fluid_player_stop(player);
        delete_fluid_player(player);
        player = nullptr;
    }
}

bool AudioStreamPlaybackFluidSynth::_is_playing() const {
    return active;
}

int32_t AudioStreamPlaybackFluidSynth::_get_loop_count() const {
    return 0;
}

double AudioStreamPlaybackFluidSynth::_get_playback_position() const {
    if (player == nullptr) {
        return 0.0;
    }
    int division = fluid_player_get_division(player);
    int bpm = fluid_player_get_bpm(player);
    int tick = fluid_player_get_current_tick(player);
    if (division <= 0 || bpm <= 0) {
        return 0.0;
    }
    return double(tick) * 60.0 / double(division * bpm);
}

void AudioStreamPlaybackFluidSynth::_seek(double p_position) {
    if (player != nullptr) {
        fluid_player_seek(player, int(p_position));
    }
}

int32_t AudioStreamPlaybackFluidSynth::_mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) {
    if (!active || base.is_null() || base->synth == nullptr) {
        return 0;
    }

    // A finished SMF transport ends the stream; Godot then emits "finished".
    if (player != nullptr && fluid_player_get_status(player) == FLUID_PLAYER_DONE) {
        delete_fluid_player(player);
        player = nullptr;
        active = false;
        return 0;
    }

    int64_t to_fill = p_frames;
    if (to_fill > 44100) {
        to_fill = 44100;
    }

    int result = fluid_synth_write_float(base->synth, to_fill, buffer, 0, 2, buffer, 1, 2);
    if (result == FLUID_FAILED) {
        return 0;
    }

    int index = 0;
    int p_index = 0;
    while (to_fill > 0) {
        p_buffer[p_index].left = buffer[index];
        p_buffer[p_index].right = buffer[index + 1];
        index += 2;
        p_index += 1;
        to_fill -= 1;
    }
    return p_frames;
}

void AudioStreamPlaybackFluidSynth::_bind_methods() {
}
