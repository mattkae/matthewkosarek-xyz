# matthewkosarek.xyz
This contains the entirety of https://matthewkosarek.xyz, which is my personal website. Feel free to look around and take any snippets that you like from it!

If you are on Github, this is a mirror of: https://git.matthewkosarek.xyz/matthew_kosarek_xyz/

## Prerequisites
- Node.js (for Astro)
- Emacs (for publishing org-mode posts)
- Meson + Emscripten (for building the WebAssembly themes)

## Building

### Everything
```sh
make build
```
This runs the posts, themes, and Astro build in sequence. Output goes to `dist/`.

### Themes only
See the [themes subproject](./themes/README.md) for more information on how to build the themes, or run:
```sh
make copy-themes
```

## Development
```sh
npm install   # first time only
make dev
# visit http://localhost:4321
```
`make dev` regenerates post fragments then starts the Astro dev server with hot reload.

## Adding a Post
1. Create a new `.org` file in `_posts/` with `#+TITLE:`, `#+DATE:`, and `#+filetags:` headers
2. Run `make posts` to generate the HTML fragment
3. The post will appear at `/posts/<filename-without-extension>`

## Publishing
```sh
./upload.sh
```
