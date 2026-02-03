.PHONY: all build clean install

all: build

build:
	make -C src/extension-code
	make -C src/updater-device-code
	python3 tools/firmware-patcher/patcher.py src/patches.toml

clean:
	make -C src/extension-code clean
	make -C src/updater-device-code clean

install:
	