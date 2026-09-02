#ifndef AUDIOSTREAMPLAYBACKFLUIDSYNTH_H
#define AUDIOSTREAMPLAYBACKFLUIDSYNTH_H

#include <fluidsynth.h>

#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/godot.hpp>

#include "audio_stream_fluidsynth.h"

namespace godot {

// The transient render session: one per active play. Renders the stream's
// synth on the audio thread and owns the SMF transport (the fluid_player_t)
// for the duration of the play.
class AudioStreamPlaybackFluidSynth : public AudioStreamPlayback {
    GDCLASS(AudioStreamPlaybackFluidSynth, AudioStreamPlayback)
    friend class AudioStreamFluidSynth;

private:
    Ref<AudioStreamFluidSynth> base;
    fluid_player_t *player;
    float *buffer;
    bool active;

public:
    AudioStreamPlaybackFluidSynth();
    ~AudioStreamPlaybackFluidSynth();

    virtual void _start(double p_from_pos) override;
    virtual void _stop() override;
    virtual bool _is_playing() const override;
    virtual int32_t _get_loop_count() const override;
    virtual double _get_playback_position() const override;
    virtual void _seek(double p_position) override;
    virtual int32_t _mix(AudioFrame *p_buffer, float p_rate_scale, int32_t p_frames) override;

    // Internal plumbing for the player node's transport; not script-visible.
    fluid_player_t *get_player() { return player; }

    static void _bind_methods();
};

} // namespace godot

#endif // AUDIOSTREAMPLAYBACKFLUIDSYNTH_H
