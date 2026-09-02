#include "audio_stream_player_fluidsynth.h"

using namespace godot;

AudioStreamPlayerFluidSynth::AudioStreamPlayerFluidSynth() : loop_count(-1) {
}

AudioStreamPlayerFluidSynth::~AudioStreamPlayerFluidSynth() {
}

fluid_synth_t *AudioStreamPlayerFluidSynth::_synth() {
    Ref<AudioStream> s = get_stream();
    Ref<AudioStreamFluidSynth> fluid = s;
    if (fluid.is_valid()) {
        return fluid->get_synth();
    }
    return nullptr;
}

fluid_player_t *AudioStreamPlayerFluidSynth::_player() {
    if (is_playing()) {
        Ref<AudioStreamPlayback> playback = get_stream_playback();
        Ref<AudioStreamPlaybackFluidSynth> fluid = playback;
        if (fluid.is_valid()) {
            return fluid->get_player();
        }
    }
    return nullptr;
}

void AudioStreamPlayerFluidSynth::program_select(int chan, int bank_num, int preset_num) {
    fluid_synth_t *s = _synth();
    if (s == nullptr) {
        return;
    }
    Ref<AudioStreamFluidSynth> fluid = get_stream();
    int sfont_id = fluid.is_valid() ? fluid->get_sfont_id() : -1;
    fluid_synth_program_select(s, chan, sfont_id, bank_num, preset_num);
}

void AudioStreamPlayerFluidSynth::note_on(int chan, int key, int vel) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_noteon(s, chan, key, vel);
    }
}

void AudioStreamPlayerFluidSynth::note_off(int chan, int key) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_noteoff(s, chan, key);
    }
}

void AudioStreamPlayerFluidSynth::cc(int chan, int ctrl, int val) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_cc(s, chan, ctrl, val);
    }
}

void AudioStreamPlayerFluidSynth::pitch_bend(int chan, int val) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_pitch_bend(s, chan, val);
    }
}

void AudioStreamPlayerFluidSynth::program_change(int chan, int program) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_program_change(s, chan, program);
    }
}

void AudioStreamPlayerFluidSynth::bank_select(int chan, int bank) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_bank_select(s, chan, bank);
    }
}

void AudioStreamPlayerFluidSynth::sfont_select(int chan, int sfont_id) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_sfont_select(s, chan, sfont_id);
    }
}

void AudioStreamPlayerFluidSynth::key_pressure(int chan, int key, int val) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_key_pressure(s, chan, key, val);
    }
}

void AudioStreamPlayerFluidSynth::channel_pressure(int chan, int val) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_channel_pressure(s, chan, val);
    }
}

void AudioStreamPlayerFluidSynth::all_notes_off(int chan) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_all_notes_off(s, chan);
    }
}

void AudioStreamPlayerFluidSynth::all_sounds_off(int chan) {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_all_sounds_off(s, chan);
    }
}

void AudioStreamPlayerFluidSynth::system_reset() {
    fluid_synth_t *s = _synth();
    if (s != nullptr) {
        fluid_synth_system_reset(s);
    }
}

int AudioStreamPlayerFluidSynth::get_cc(int chan, int ctrl) {
    fluid_synth_t *s = _synth();
    if (s == nullptr) {
        return 0;
    }
    int val = 0;
    fluid_synth_get_cc(s, chan, ctrl, &val);
    return val;
}

int AudioStreamPlayerFluidSynth::get_pitch_bend(int chan) {
    fluid_synth_t *s = _synth();
    if (s == nullptr) {
        return 0;
    }
    int val = 0;
    fluid_synth_get_pitch_bend(s, chan, &val);
    return val;
}

int AudioStreamPlayerFluidSynth::get_pitch_wheel_sens(int chan) {
    fluid_synth_t *s = _synth();
    if (s == nullptr) {
        return 0;
    }
    int val = 0;
    fluid_synth_get_pitch_wheel_sens(s, chan, &val);
    return val;
}

int AudioStreamPlayerFluidSynth::get_program(int chan) {
    fluid_synth_t *s = _synth();
    if (s == nullptr) {
        return 0;
    }
    int sfont_id = -1;
    int bank_num = 0;
    int preset_num = 0;
    fluid_synth_get_program(s, chan, &sfont_id, &bank_num, &preset_num);
    return preset_num;
}

int AudioStreamPlayerFluidSynth::get_voice_count() {
    fluid_synth_t *s = _synth();
    if (s == nullptr) {
        return 0;
    }
    return fluid_synth_get_active_voice_count(s);
}

void AudioStreamPlayerFluidSynth::set_loop(int loops) {
    loop_count = loops;
    fluid_player_t *p = _player();
    if (p != nullptr) {
        fluid_player_set_loop(p, loops);
    }
}

int AudioStreamPlayerFluidSynth::get_loop() const {
    return loop_count;
}

int AudioStreamPlayerFluidSynth::get_tick() {
    fluid_player_t *p = _player();
    return p != nullptr ? fluid_player_get_current_tick(p) : 0;
}

int AudioStreamPlayerFluidSynth::get_total_ticks() {
    fluid_player_t *p = _player();
    return p != nullptr ? fluid_player_get_total_ticks(p) : 0;
}

int AudioStreamPlayerFluidSynth::get_bpm() {
    fluid_player_t *p = _player();
    return p != nullptr ? fluid_player_get_bpm(p) : 0;
}

int AudioStreamPlayerFluidSynth::get_division() {
    fluid_player_t *p = _player();
    return p != nullptr ? fluid_player_get_division(p) : 0;
}

void AudioStreamPlayerFluidSynth::set_tempo(int tempo_type, double tempo) {
    fluid_player_t *p = _player();
    if (p != nullptr) {
        fluid_player_set_tempo(p, tempo_type, tempo);
    }
}

double AudioStreamPlayerFluidSynth::get_tempo() {
    fluid_player_t *p = _player();
    return p != nullptr ? double(fluid_player_get_bpm(p)) : 0.0;
}

void AudioStreamPlayerFluidSynth::_bind_methods() {
    ClassDB::bind_method(D_METHOD("program_select", "chan", "bank_num", "preset_num"),
                         &AudioStreamPlayerFluidSynth::program_select);
    ClassDB::bind_method(D_METHOD("note_on", "chan", "key", "vel"), &AudioStreamPlayerFluidSynth::note_on);
    ClassDB::bind_method(D_METHOD("note_off", "chan", "key"), &AudioStreamPlayerFluidSynth::note_off);
    ClassDB::bind_method(D_METHOD("cc", "chan", "ctrl", "val"), &AudioStreamPlayerFluidSynth::cc);
    ClassDB::bind_method(D_METHOD("pitch_bend", "chan", "val"), &AudioStreamPlayerFluidSynth::pitch_bend);
    ClassDB::bind_method(D_METHOD("program_change", "chan", "program"), &AudioStreamPlayerFluidSynth::program_change);
    ClassDB::bind_method(D_METHOD("bank_select", "chan", "bank"), &AudioStreamPlayerFluidSynth::bank_select);
    ClassDB::bind_method(D_METHOD("sfont_select", "chan", "sfont_id"), &AudioStreamPlayerFluidSynth::sfont_select);
    ClassDB::bind_method(D_METHOD("key_pressure", "chan", "key", "val"), &AudioStreamPlayerFluidSynth::key_pressure);
    ClassDB::bind_method(D_METHOD("channel_pressure", "chan", "val"), &AudioStreamPlayerFluidSynth::channel_pressure);
    ClassDB::bind_method(D_METHOD("all_notes_off", "chan"), &AudioStreamPlayerFluidSynth::all_notes_off);
    ClassDB::bind_method(D_METHOD("all_sounds_off", "chan"), &AudioStreamPlayerFluidSynth::all_sounds_off);
    ClassDB::bind_method(D_METHOD("system_reset"), &AudioStreamPlayerFluidSynth::system_reset);

    ClassDB::bind_method(D_METHOD("get_cc", "chan", "ctrl"), &AudioStreamPlayerFluidSynth::get_cc);
    ClassDB::bind_method(D_METHOD("get_pitch_bend", "chan"), &AudioStreamPlayerFluidSynth::get_pitch_bend);
    ClassDB::bind_method(D_METHOD("get_pitch_wheel_sens", "chan"), &AudioStreamPlayerFluidSynth::get_pitch_wheel_sens);
    ClassDB::bind_method(D_METHOD("get_program", "chan"), &AudioStreamPlayerFluidSynth::get_program);
    ClassDB::bind_method(D_METHOD("get_voice_count"), &AudioStreamPlayerFluidSynth::get_voice_count);

    ClassDB::bind_method(D_METHOD("set_loop", "loops"), &AudioStreamPlayerFluidSynth::set_loop);
    ClassDB::bind_method(D_METHOD("get_loop"), &AudioStreamPlayerFluidSynth::get_loop);
    ClassDB::add_property("AudioStreamPlayerFluidSynth", PropertyInfo(Variant::INT, "loop"), "set_loop", "get_loop");
    ClassDB::bind_method(D_METHOD("get_tick"), &AudioStreamPlayerFluidSynth::get_tick);
    ClassDB::bind_method(D_METHOD("get_total_ticks"), &AudioStreamPlayerFluidSynth::get_total_ticks);
    ClassDB::bind_method(D_METHOD("get_bpm"), &AudioStreamPlayerFluidSynth::get_bpm);
    ClassDB::bind_method(D_METHOD("get_division"), &AudioStreamPlayerFluidSynth::get_division);
    ClassDB::bind_method(D_METHOD("set_tempo", "tempo_type", "tempo"), &AudioStreamPlayerFluidSynth::set_tempo);
    ClassDB::bind_method(D_METHOD("get_tempo"), &AudioStreamPlayerFluidSynth::get_tempo);
}
