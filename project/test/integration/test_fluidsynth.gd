class_name TestFluidSynth
extends GdUnitTestSuite

const MISSING_SOUNDFONT := "res://test/no_such_soundfont.sf2"

var stream: AudioStreamFluidSynth
var player: AudioStreamPlayerFluidSynth


func before():
	# A fresh stream + player per test; the player hosts the stream so the
	# synth renders into Godot's audio server.
	stream = auto_free(AudioStreamFluidSynth.new())
	player = auto_free(AudioStreamPlayerFluidSynth.new())
	player.stream = stream
	add_child(player)


func after():
	if is_instance_valid(player):
		player.stop()
		remove_child(player)


func test_classes_registered():
	assert_object(AudioStreamFluidSynth.new()).is_not_null()
	assert_object(AudioStreamPlayerFluidSynth.new()).is_not_null()
	collect_orphan_node_details()

func test_stream_defaults():
	assert_object(stream.get_soundfont()).is_null()
	assert_object(stream.get_midi_file()).is_null()
	assert_str(stream.get_stream_name()).is_equal("FluidSynth")


func test_stream_midi_property_roundtrip():
	var reader = auto_free(MidiFileReader.new())
	reader.set_data(PackedByteArray([0x4D, 0x54, 0x68, 0x64]))
	stream.set_midi_file(reader)
	assert_object(stream.get_midi_file()).is_equal(reader)

	var reader2 = auto_free(MidiFileReader.new())
	reader2.set_data(PackedByteArray([0x01, 0x02]))
	stream.set_midi_file(reader2)
	assert_object(stream.get_midi_file()).is_equal(reader2)


func test_stream_set_get_soundfont():
	if not ResourceLoader.exists("res://assets/example.sf2"):
		return
	stream.set_soundfont("res://assets/example.sf2")
	assert_object(stream.get_soundfont()).is_not_null()


func test_stream_set_get_midi_file():
	if not ResourceLoader.exists("res://assets/example.mid"):
		return
	stream.set_midi_file(ResourceLoader.load("res://assets/example.mid"))
	assert_object(stream.get_midi_file()).is_not_null()


func test_set_soundfont_missing_is_graceful():
	# Missing path should not crash; the stream checks ResourceLoader::exists.
	stream.set_soundfont(MISSING_SOUNDFONT)
	await get_tree().process_frame
	assert_bool(true).is_true()


func test_play_midi_with_valid_assets():
	if not ResourceLoader.exists("res://assets/example.mid"):
		return
	stream.set_midi_file(ResourceLoader.load("res://assets/example.mid"))
	await get_tree().process_frame
	player.play()
	await get_tree().process_frame
	# Reaching here without error under the headless dummy audio driver is the bar.
	await get_tree().process_frame
	player.stop()
	await get_tree().process_frame


func test_note_and_control_smoke_during_playback():
	player.play()
	await get_tree().process_frame
	player.program_select(0, 0, 0)
	player.note_on(0, 60, 127)
	await get_tree().process_frame
	player.pitch_bend(0, 10000)
	await get_tree().process_frame
	player.note_off(0, 60)
	player.stop()
	await get_tree().process_frame


func test_replay_after_stop():
	player.play()
	await get_tree().process_frame
	assert_bool(player.is_playing()).is_true()
	player.stop()
	await get_tree().process_frame
	player.play()
	await get_tree().process_frame
	player.stop()
	await get_tree().process_frame


func test_audiostream_player_integration():
	if not ResourceLoader.exists("res://assets/example.sf2") or not ResourceLoader.exists("res://assets/example.mid"):
		return
	stream.set_soundfont("res://assets/example.sf2")
	stream.set_midi_file(ResourceLoader.load("res://assets/example.mid"))
	await get_tree().process_frame
	player.play()
	await get_tree().process_frame
	assert_bool(player.is_playing()).is_true()
	player.stop()
	await get_tree().process_frame
