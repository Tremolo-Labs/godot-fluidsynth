UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
PLATFORM=linux
endif
ifeq ($(UNAME), Darwin)
PLATFORM=osx
endif

all:
	scons platform=$(PLATFORM) bits=64

.PHONY: godot-cpp

godot-cpp:
	(cd godot-cpp && scons platform=$(PLATFORM) bits=64 generate_bindings=yes)

.PHONY: assets
assets:
	cp /usr/share/soundfonts/FluidR3_GM.sf2 project/assets/example.sf2
	abc2midi project/assets/example.abc -o project/assets/example.mid
