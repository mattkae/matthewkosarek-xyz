.PHONY: all themes copy-themes build dev clean

all: build

themes/builddir:
	meson setup themes themes/builddir --cross-file themes/emscripten.ini

themes: themes/builddir
	meson compile -C themes/builddir

copy-themes: themes
	mkdir -p ./public/themes/dist
	rsync -a themes/dist/ ./public/themes/dist/
	mkdir -p ./public/themes/src/_shaders
	rsync -a themes/src/_shaders/ ./public/themes/src/_shaders/

build: copy-themes
	npm run build

dev:
	npm run dev

clean:
	rm -rf dist public/themes
