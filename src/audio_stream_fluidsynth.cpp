#include "audio_stream_fluidsynth.h"

#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include "audio_stream_playback_fluidsynth.h"

using namespace godot;

AudioStreamFluidSynth::AudioStreamFluidSynth() : settings(nullptr), synth(nullptr), sfont_id(-1) {
    settings = new_fluid_settings();
    // The sample rate is fixed at synth construction; change it before this
    // point if the default is not right for the project.
    fluid_settings_setnum(settings, "synth.sample-rate", 44100.0);
    synth = new_fluid_synth(settings);

    fluid_sfloader_t *sfloader = new_fluid_defsfloader(settings);
    fluid_sfloader_set_callbacks(sfloader, SoundFontFileReader::sf_open, SoundFontFileReader::sf_read,
                                 SoundFontFileReader::sf_seek, SoundFontFileReader::sf_tell,
                                 SoundFontFileReader::sf_close);
    fluid_synth_add_sfloader(synth, sfloader);
}

AudioStreamFluidSynth::~AudioStreamFluidSynth() {
    if (synth != nullptr) {
        delete_fluid_synth(synth);
    }
    if (settings != nullptr) {
        delete_fluid_settings(settings);
    }
}

String AudioStreamFluidSynth::_get_stream_name() const {
    return "FluidSynth";
}

Ref<AudioStreamPlayback> AudioStreamFluidSynth::_instantiate_playback() const {
    Ref<AudioStreamPlaybackFluidSynth> playback;
    playback.instantiate();
    playback->base = Ref<AudioStreamFluidSynth>(this);
    return playback;
}

void AudioStreamFluidSynth::set_soundfont(String p_path) {
    if (!ResourceLoader::get_singleton()->exists(p_path)) {
        return;
    }
    Variant resource = ResourceLoader::get_singleton()->load(p_path);
    Ref<SoundFontFileReader> sf = resource;
    if (sf.is_null()) {
        return;
    }
    // Keep the resource alive for the synth's lazy sample cache and encode the
    // address of this Ref as the pseudo-path the sfloader decodes back into it.
    soundfont = sf;
    char abused_filename[64];
    const void *pointer_to_sf2_in_mem = static_cast<void *>(&soundfont);
    snprintf(abused_filename, sizeof(abused_filename), "&%p", pointer_to_sf2_in_mem);
    sfont_id = fluid_synth_sfload(synth, abused_filename, 1);
}

Ref<SoundFontFileReader> AudioStreamFluidSynth::get_soundfont() {
    return soundfont;
}

void AudioStreamFluidSynth::set_midi_file(Ref<MidiFileReader> p_midi_file) {
    midi_file = p_midi_file;
}

Ref<MidiFileReader> AudioStreamFluidSynth::get_midi_file() {
    return midi_file;
}

void AudioStreamFluidSynth::set_sample_rate(int p_rate) {
    fluid_settings_setnum(settings, "synth.sample-rate", p_rate);
}

int AudioStreamFluidSynth::get_sample_rate() const {
    double val = 44100.0;
    fluid_settings_getnum(settings, "synth.sample-rate", &val);
    return int(val);
}

void AudioStreamFluidSynth::set_gain(double p_gain) {
    fluid_settings_setnum(settings, "synth.gain", p_gain);
}

double AudioStreamFluidSynth::get_gain() const {
    double val = 0.2;
    fluid_settings_getnum(settings, "synth.gain", &val);
    return val;
}

void AudioStreamFluidSynth::set_polyphony(int p_polyphony) {
    fluid_settings_setnum(settings, "synth.polyphony", p_polyphony);
}

int AudioStreamFluidSynth::get_polyphony() const {
    double val = 256.0;
    fluid_settings_getnum(settings, "synth.polyphony", &val);
    return int(val);
}

void AudioStreamFluidSynth::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont"), &AudioStreamFluidSynth::set_soundfont);
    ClassDB::bind_method(D_METHOD("get_soundfont"), &AudioStreamFluidSynth::get_soundfont);
    ClassDB::add_property("AudioStreamFluidSynth",
                          PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "SoundFontFileReader"),
                          "set_soundfont", "get_soundfont");

    ClassDB::bind_method(D_METHOD("set_midi_file", "midi_file"), &AudioStreamFluidSynth::set_midi_file);
    ClassDB::bind_method(D_METHOD("get_midi_file"), &AudioStreamFluidSynth::get_midi_file);
    ClassDB::add_property("AudioStreamFluidSynth",
                          PropertyInfo(Variant::OBJECT, "midi_file", PROPERTY_HINT_RESOURCE_TYPE, "MidiFileReader"),
                          "set_midi_file", "get_midi_file");

    ClassDB::bind_method(D_METHOD("set_sample_rate", "sample_rate"), &AudioStreamFluidSynth::set_sample_rate);
    ClassDB::bind_method(D_METHOD("get_sample_rate"), &AudioStreamFluidSynth::get_sample_rate);
    ClassDB::add_property("AudioStreamFluidSynth", PropertyInfo(Variant::INT, "sample_rate"), "set_sample_rate",
                          "get_sample_rate");

    ClassDB::bind_method(D_METHOD("set_gain", "gain"), &AudioStreamFluidSynth::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &AudioStreamFluidSynth::get_gain);
    ClassDB::add_property("AudioStreamFluidSynth", PropertyInfo(Variant::FLOAT, "gain"), "set_gain", "get_gain");

    ClassDB::bind_method(D_METHOD("set_polyphony", "polyphony"), &AudioStreamFluidSynth::set_polyphony);
    ClassDB::bind_method(D_METHOD("get_polyphony"), &AudioStreamFluidSynth::get_polyphony);
    ClassDB::add_property("AudioStreamFluidSynth", PropertyInfo(Variant::INT, "polyphony"), "set_polyphony",
                          "get_polyphony");
}
