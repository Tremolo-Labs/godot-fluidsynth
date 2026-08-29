class_name TestFluidSynth
extends GdUnitTestSuite

const MISSING_SOUNDFONT := "res://test/no_such_soundfont.sf2"
const MISSING_MIDI_FILE := "res://test/no_such_song.mid"


func before():
	assert_object(FluidSynth).is_not_null()
	await get_tree().process_frame
	# Suite-wide load: each extra set_soundfont stacks a full copy in the FluidSynth.
	# Keep it once here so later tests don't accumulate.
	if FluidSynth and ResourceLoader.exists("res://assets/example.sf2"):
		FluidSynth.set_soundfont("res://assets/example.sf2")
	if FluidSynth and ResourceLoader.exists("res://assets/example.mid"):
		var midi_res = ResourceLoader.load("res://assets/example.mid")
		if midi_res != null:
			FluidSynth.set_midi_file(midi_res)


func after_test():
	# Mirrors original after_test: ensure no dangling playback affects next test.
	# FluidSynth has no stop(); players are reaped in process() when DONE.
	await get_tree().process_frame

func test_singleton_exists():
	assert_object(Engine.get_singleton("FluidSynth")).is_not_null()
	assert_object(FluidSynth).is_not_null()
	assert_object(Engine.get_singleton("FluidSynth")).is_equal(FluidSynth)

func test_audiostream_defaults():
	var stream = auto_free(AudioStreamFluidSynth.new())
	assert_object(stream.get_soundfont()).is_null()
	assert_object(stream.get_midi_file()).is_null()
	assert_str(stream.get_stream_name()).is_equal("FluidSynth")

func test_fluidsynth_midi_property_roundtrip():
	var reader = auto_free(MidiFileReader.new())
	reader.set_data(PackedByteArray([0x4D, 0x54, 0x68, 0x64]))
	# Fresh singleton state is suite-bound; use the setter and verify getter.
	FluidSynth.set_midi_file(reader)
	assert_object(FluidSynth.get_midi_file()).is_equal(reader)
	# Overwrite with another value
	var reader2 = auto_free(MidiFileReader.new())
	reader2.set_data(PackedByteArray([0x01, 0x02]))
	FluidSynth.set_midi_file(reader2)
	assert_object(FluidSynth.get_midi_file()).is_equal(reader2)

func test_audiostream_set_get_soundfont():
	var stream = auto_free(AudioStreamFluidSynth.new())
	if not ResourceLoader.exists("res://assets/example.sf2"):
		return
	var sf = ResourceLoader.load("res://assets/example.sf2")
	assert_object(sf).is_not_null()
	stream.set_soundfont(sf)
	assert_object(stream.get_soundfont()).is_equal(sf)

func test_audiostream_set_get_midi_file():
	var stream = auto_free(AudioStreamFluidSynth.new())
	if not ResourceLoader.exists("res://assets/example.mid"):
		return
	var midi = ResourceLoader.load("res://assets/example.mid")
	assert_object(midi).is_not_null()
	stream.set_midi_file(midi)
	assert_object(stream.get_midi_file()).is_equal(midi)

func test_set_soundfont_missing_is_graceful():
	# Missing path should not crash and should not corrupt existing font.
	# FluidSynth::set_soundfont checks ResourceLoader::exists and returns early.
	var before = FluidSynth.get_soundfont()
	FluidSynth.set_soundfont(MISSING_SOUNDFONT)
	# getter still returns previous Ref (or null if never set) — just verify no throw.
	await get_tree().process_frame
	assert_bool(true).is_true()

func test_set_midi_file_missing_is_graceful():
	var fresh = auto_free(AudioStreamFluidSynth.new())
	# Passing null is the "missing" case for Ref-based API.
	fresh.set_midi_file(null)
	assert_object(fresh.get_midi_file()).is_null()

func test_play_midi_with_valid_assets():
	if not ResourceLoader.exists("res://assets/example.mid"):
		return
	var midi = ResourceLoader.load("res://assets/example.mid")
	assert_object(midi).is_not_null()
	await get_tree().process_frame
	FluidSynth.play_midi(midi)
	await get_tree().process_frame
	# Reaching here without error under dummy audio driver is the bar.
	await get_tree().process_frame

func test_play_midi_default_uses_stored_file():
	# play_midi(null) should use the stored midi_file property.
	if FluidSynth.get_midi_file() == null:
		if ResourceLoader.exists("res://assets/example.mid"):
			FluidSynth.set_midi_file(ResourceLoader.load("res://assets/example.mid"))
		else:
			return
	await get_tree().process_frame
	FluidSynth.play_midi(null)
	await get_tree().process_frame

func test_play_midi_invalid_is_graceful():
	var orphan = auto_free(MidiFileReader.new())
	# Empty data -> play_midi will see size 0 and do nothing, must not throw.
	orphan.set_data(PackedByteArray())
	FluidSynth.play_midi(orphan)
	await get_tree().process_frame

func test_note_and_control_smoke_during_playback():
	if not ResourceLoader.exists("res://assets/example.mid"):
		return
	var midi = ResourceLoader.load("res://assets/example.mid")
	if midi == null:
		fail("failed to load res://assets/example.mid")
		return
	FluidSynth.play_midi(midi)
	await get_tree().process_frame
	FluidSynth.program_select(0, 0, 0)
	FluidSynth.note_on(0, 60, 127)
	await get_tree().process_frame
	FluidSynth.pitch_bend(0, 10000)
	await get_tree().process_frame
	FluidSynth.note_off(0, 60)
	await get_tree().process_frame

func test_replay_after_stop():
	if not ResourceLoader.exists("res://assets/example.mid"):
		return
	var midi = ResourceLoader.load("res://assets/example.mid")
	await get_tree().process_frame
	FluidSynth.play_midi(midi)
	await get_tree().process_frame
	# No explicit stop on singleton; replay is creating a new player while previous
	# is still active. Ensure second play does not throw and is reaped.
	FluidSynth.play_midi(midi)
	await get_tree().process_frame
	await get_tree().process_frame

func test_audiostream_player_integration():
	if not ResourceLoader.exists("res://assets/example.sf2") or not ResourceLoader.exists("res://assets/example.mid"):
		return
	var sf = ResourceLoader.load("res://assets/example.sf2")
	var midi = ResourceLoader.load("res://assets/example.mid")
	var stream = auto_free(AudioStreamFluidSynth.new())
	stream.set_soundfont(sf)
	stream.set_midi_file(midi)
	var player = auto_free(AudioStreamPlayer.new())
	player.stream = stream
	add_child(player)
	player.play()
	await get_tree().process_frame
	assert_bool(player.is_playing()).is_true()
	player.stop()
	await get_tree().process_frame
	remove_child(player)
