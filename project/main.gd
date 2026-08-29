extends Node2D

var fluidsynth: Node

@export var midi_file: MidiFileReader

func _ready() -> void:
	fluidsynth = Engine.get_singleton("FluidSynth")


func _on_fluidsynth_ready() -> void:
	print("fluidsynth _ready")
	fluidsynth.call("play_midi")

func _on_CheckButton_toggled(button_pressed: bool) -> void:
	if button_pressed:
		fluidsynth.call("note_on", 1, 60, 90)
	else:
		fluidsynth.call("note_off", 1, 60)

func _on_CheckButton2_toggled(button_pressed: bool) -> void:
	if button_pressed:
		fluidsynth.call("note_on", 1, 64, 90)
	else:
		fluidsynth.call("note_off", 1, 64)

func _on_CheckButton3_toggled(button_pressed: bool) -> void:
	if button_pressed:
		fluidsynth.call("note_on", 13, 67, 90)
	else:
		fluidsynth.call("note_off", 13, 67)

func _on_v_slider_value_changed(value: float) -> void:
	fluidsynth.call("pitch_bend", 1, value)

func _on_button_pressed() -> void:
	fluidsynth.call("play_midi", midi_file)
