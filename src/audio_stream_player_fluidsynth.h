#ifndef AUDIOSTREAMPLAYERFLUIDSYNTH_H
#define AUDIOSTREAMPLAYERFLUIDSYNTH_H

#include <fluidsynth.h>

#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/godot.hpp>

#include "audio_stream_fluidsynth.h"
#include "audio_stream_playback_fluidsynth.h"

namespace godot {

// The instance node: an AudioStreamPlayer whose stream is an
// AudioStreamFluidSynth. Owns the lifecycle (play/stop/seek/finished), the
// SMF transport controls, and the voice-control surface, which it forwards to
// the stream's synth.
class AudioStreamPlayerFluidSynth : public AudioStreamPlayer {
    GDCLASS(AudioStreamPlayerFluidSynth, AudioStreamPlayer)

private:
    int loop_count;

    fluid_synth_t *_synth();
    fluid_player_t *_player();

protected:
    static void _bind_methods();

public:
    AudioStreamPlayerFluidSynth();
    ~AudioStreamPlayerFluidSynth();

    // Controls, forwarded to the stream's synth.
    void program_select(int chan, int bank_num, int preset_num);
    void note_on(int chan, int key, int vel);
    void note_off(int chan, int key);
    void cc(int chan, int ctrl, int val);
    void pitch_bend(int chan, int val);
    void program_change(int chan, int program);
    void bank_select(int chan, int bank);
    void sfont_select(int chan, int sfont_id);
    void key_pressure(int chan, int key, int val);
    void channel_pressure(int chan, int val);
    void all_notes_off(int chan);
    void all_sounds_off(int chan);
    void system_reset();

    // State queries.
    int get_cc(int chan, int ctrl);
    int get_pitch_bend(int chan);
    int get_pitch_wheel_sens(int chan);
    int get_program(int chan);
    int get_voice_count();

    // SMF transport.
    void set_loop(int loops);
    int get_loop() const;
    int get_tick();
    int get_total_ticks();
    int get_bpm();
    int get_division();
    void set_tempo(int tempo_type, double tempo);
    double get_tempo();
};

} // namespace godot

#endif // AUDIOSTREAMPLAYERFLUIDSYNTH_H
