class_name TestSoundFontFileReader
extends GdUnitTestSuite

func test_create_and_get_data():
	var reader = SoundFontFileReader.new()
	var data = PackedByteArray([0x52, 0x49, 0x46, 0x46])
	reader.set_data(data)
	var result = reader.get_data()
	assert_array(result).has_size(4)
	assert_str(reader.get_extension()).is_equal("sf2str")

func test_empty_data():
	var reader = SoundFontFileReader.new()
	var result = reader.get_data()
	assert_array(result).is_empty()

func test_get_extension():
	var reader = SoundFontFileReader.new()
	assert_str(reader.get_extension()).is_equal("sf2str")

func test_real_asset_roundtrip():
	var f = FileAccess.open("res://assets/example.sf2", FileAccess.READ)
	if f == null:
		fail("res://assets/example.sf2 missing — run 'make assets' first")
		return
	var original = f.get_buffer(f.get_length())
	f.close()
	assert_int(original.size()).is_greater(0)

	var reader = SoundFontFileReader.new()
	reader.set_data(original)
	var result = reader.get_data()
	assert_int(result.size()).is_equal(original.size())
	if result != original:
		fail("roundtrip mismatch")

func test_real_asset_has_riff_sfbk_header():
	var f = FileAccess.open("res://assets/example.sf2", FileAccess.READ)
	if f == null:
		fail("res://assets/example.sf2 missing — run 'make assets' first")
		return
	var reader = SoundFontFileReader.new()
	reader.set_data(f.get_buffer(f.get_length()))
	f.close()
	var data = reader.get_data()
	# RIFF layout: "RIFF" | uint32 LE chunk size (bytes 4..7) | form type (bytes 8..11)
	assert_int(data[0]).is_equal(0x52) # 'R'
	assert_int(data[1]).is_equal(0x49) # 'I'
	assert_int(data[2]).is_equal(0x46) # 'F'
	assert_int(data[3]).is_equal(0x46) # 'F'
	assert_int(data.size() - 8).is_greater(0)
	assert_int(data[4] + (data[5] << 8) + (data[6] << 16) + (data[7] << 24)).is_equal(data.size() - 8)
	assert_int(data[8]).is_equal(0x73) # 's'
	assert_int(data[9]).is_equal(0x66) # 'f'
	assert_int(data[10]).is_equal(0x62) # 'b'
	assert_int(data[11]).is_equal(0x6B) # 'k'

func test_set_data_overwrites_previous():
	var reader = SoundFontFileReader.new()
	reader.set_data(PackedByteArray([0x01]))
	reader.set_data(PackedByteArray([0x02, 0x03, 0x04]))
	assert_array(reader.get_data()).has_size(3)
