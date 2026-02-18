.PHONY: all posts themes copy clean

all: copy

posts:
	mkdir -p ./public/posts
	emacs -Q --script publish.el

themes/builddir:
	meson setup themes/builddir --cross-file themes/emscripten.ini

themes: themes/builddir
	meson compile -C themes/builddir

copy: posts themes
	mkdir -p ./public/themes/dist
	rsync -a themes/dist/ ./public/themes/dist/

clean:
	rm -rf public/themes
