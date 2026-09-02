extends Node2D

@onready var fluidsynth: AudioStreamPlayerFluidSynth = $FluidSynthPlayer

func _on_CheckButton_toggled(button_pressed: bool) -> void:
	if button_pressed:
		fluidsynth.note_on(1, 60, 90)
	else:
		fluidsynth.note_off(1, 60)

func _on_CheckButton2_toggled(button_pressed: bool) -> void:
	if button_pressed:
		fluidsynth.note_on(1, 64, 90)
	else:
		fluidsynth.note_off(1, 64)

func _on_CheckButton3_toggled(button_pressed: bool) -> void:
	if button_pressed:
		fluidsynth.note_on(13, 67, 90)
	else:
		fluidsynth.note_off(13, 67)

func _on_v_slider_value_changed(value: float) -> void:
	fluidsynth.pitch_bend(1, int(value))

func _on_button_pressed() -> void:
	fluidsynth.play()
