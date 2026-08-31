#ifndef AUDIOSTREAMFLUIDSYNTH_H
#define AUDIOSTREAMFLUIDSYNTH_H

#include <fluidsynth.h>

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/godot.hpp>

#include "midi_file_reader.h"
#include "soundfont_file_reader.h"

namespace godot {

class AudioStreamPlaybackFluidSynth;

// The FluidSynth instance: owns the settings and the synth. A pure audio
// source — no transport and no controls, it only renders. The player node
// holds one of these as its stream and drives the synth through it.
class AudioStreamFluidSynth : public AudioStream {
    GDCLASS(AudioStreamFluidSynth, AudioStream)

    friend class AudioStreamPlaybackFluidSynth;

private:
    fluid_settings_t *settings;
    fluid_synth_t *synth;
    int sfont_id;

    Ref<SoundFontFileReader> soundfont;
    Ref<MidiFileReader> midi_file;

protected:
    static void _bind_methods();

public:
    AudioStreamFluidSynth();
    ~AudioStreamFluidSynth();

    virtual String _get_stream_name() const override;
    virtual double _get_length() const override { return 0.0; }
    virtual Ref<AudioStreamPlayback> _instantiate_playback() const override;

    void set_soundfont(String p_path);
    Ref<SoundFontFileReader> get_soundfont();
    void set_midi_file(Ref<MidiFileReader> p_midi_file);
    Ref<MidiFileReader> get_midi_file();

    void set_sample_rate(int p_rate);
    int get_sample_rate() const;
    void set_gain(double p_gain);
    double get_gain() const;
    void set_polyphony(int p_polyphony);
    int get_polyphony() const;

    // Internal plumbing for the player node's controls; not script-visible.
    fluid_synth_t *get_synth() { return synth; }
    int get_sfont_id() { return sfont_id; }
};

} // namespace godot

#endif // AUDIOSTREAMFLUIDSYNTH_H
