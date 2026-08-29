#include "audio_stream_mytone.h"
#include "audio_stream_player_mytone.h"

using namespace godot;

AudioStreamMyTone::AudioStreamMyTone() : mix_rate(44100), stereo(false), hz(639) {
}

Ref<AudioStreamPlayback> AudioStreamMyTone::_instantiate_playback() const {
    Ref<AudioStreamPlaybackMyTone> talking_tree;
    talking_tree.instantiate();
    talking_tree->base = Ref<AudioStreamMyTone>(this);
    return talking_tree;
}

String AudioStreamMyTone::get_stream_name() const {
    return "MyTone";
}

void AudioStreamMyTone::reset() {
    set_position(0);
}

void AudioStreamMyTone::set_position(uint64_t p) {
    pos = p;
}

float AudioStreamMyTone::gen_tone() {
    float inc = 1.0 / (float(mix_rate) / float(hz));
    pos += inc;
    if (pos > 1.0) {
        pos -= 1.0;
    }
    return sin(2.0 * M_PI * pos);
}

void AudioStreamMyTone::_bind_methods() {
    ClassDB::bind_method(D_METHOD("reset"), &AudioStreamMyTone::reset);
    ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamMyTone::get_stream_name);
}
