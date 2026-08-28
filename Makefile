UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
PLATFORM=linux
endif
ifeq ($(UNAME), Darwin)
PLATFORM=osx
endif

all:
	scons platform=$(PLATFORM) compiledb=yes target=template_debug

.PHONY: godot-cpp

godot-cpp:
	(cd godot-cpp && scons platform=$(PLATFORM) bits=64 generate_bindings=yes)

# First soundfont found in known locations wins; override with SOUNDFONT=/path/to/font.sf2
SOUNDFONT ?= $(firstword $(wildcard \
	/usr/share/sounds/sf2/FluidR3_GM.sf2 \
	/usr/share/soundfonts/*.sf2))

ifeq ($(SOUNDFONT),)
$(error No soundfont found in known locations; set SOUNDFONT=/path/to/font.sf2)
endif

.PHONY: assets
assets:
	cp $(SOUNDFONT) project/assets/example.sf2
	abc2midi project/assets/example.abc -o project/assets/example.mid
	abc2midi project/assets/example2.abc -o project/assets/example2.mid

.PHONY: addons
addons:
	scons addons
