# Themes
This subproject provides themes for the website.

## Prerequisites

- Emscripten SDK (emsdk)
- Meson build system (`pip install meson` or via your package manager)
- Ninja build backend (usually installed with Meson)

## Building

### Initial Setup

```bash
meson setup builddir --cross-file emscripten.ini
```

### Compile

```bash
meson compile -C builddir
```

The output files will be automatically copied to:
- `dist/output.js`
- `dist/output.wasm`

The original build artifacts are also available in `builddir/` if needed.

### Clean Build

```bash
# Clean and rebuild
meson compile -C builddir --clean
meson compile -C builddir
```

### Complete Clean

```bash
# Remove the build directory entirely
rm -rf builddir
```

## Build Options

You can configure build options during setup:

```bash
# Debug build
meson setup builddir --cross-file emscripten.ini --buildtype=debug

# Release build with optimizations
meson setup builddir --cross-file emescripten.ini --buildtype=release

# Reconfigure an existing build
meson configure builddir --buildtype=debug
```
