class_name TestMidiFileReader
extends GdUnitTestSuite

func test_create_and_get_data():
	var reader = MidiFileReader.new()
	var data = PackedByteArray([0x4D, 0x54, 0x68, 0x64])
	reader.set_data(data)
	var result = reader.get_data()
	assert_array(result).has_size(4)
	assert_str(reader.get_extension()).is_equal("midstr")

func test_empty_data():
	var reader = MidiFileReader.new()
	var result = reader.get_data()
	assert_array(result).is_empty()

func test_get_extension():
	var reader = MidiFileReader.new()
	assert_str(reader.get_extension()).is_equal("midstr")

func test_get_data_returns_independent_copy():
	var reader = MidiFileReader.new()
	var data = PackedByteArray([0x01, 0x02, 0x03])
	reader.set_data(data)
	data.resize(0)
	var result = reader.get_data()
	assert_array(result).has_size(3)

func test_real_asset_roundtrip():
	var f = FileAccess.open("res://assets/example.mid", FileAccess.READ)
	if f == null:
		fail("res://assets/example.mid missing — run 'make assets' first")
		return
	var original = f.get_buffer(f.get_length())
	f.close()
	assert_int(original.size()).is_greater(0)

	var reader = MidiFileReader.new()
	reader.set_data(original)
	var result = reader.get_data()
	assert_int(result.size()).is_equal(original.size())
	if result != original:
		fail("roundtrip mismatch")

func test_real_asset_has_mthd_header():
	var f = FileAccess.open("res://assets/example.mid", FileAccess.READ)
	if f == null:
		fail("res://assets/example.mid missing — run 'make assets' first")
		return
	var reader = MidiFileReader.new()
	reader.set_data(f.get_buffer(f.get_length()))
	f.close()
	var data = reader.get_data()
	assert_int(data[0]).is_equal(0x4D) # 'M'
	assert_int(data[1]).is_equal(0x54) # 'T'
	assert_int(data[2]).is_equal(0x68) # 'h'
	assert_int(data[3]).is_equal(0x64) # 'd'

func test_set_data_overwrites_previous():
	var reader = MidiFileReader.new()
	reader.set_data(PackedByteArray([0x01]))
	reader.set_data(PackedByteArray([0x02, 0x03, 0x04]))
	assert_array(reader.get_data()).has_size(3)

func test_binary_zero_fidelity():
	var reader = MidiFileReader.new()
	var data = PackedByteArray()
	data.resize(64)
	reader.set_data(data)
	var result = reader.get_data()
	assert_array(result).has_size(64)
	for i in range(result.size()):
		assert_int(result[i]).is_equal(0)
