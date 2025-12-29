project('themes', 'cpp',
  version: '1.0.0',
  default_options: [
    'cpp_std=c++17',
    'buildtype=release',
    'warning_level=2'
  ]
)

# Emscripten link flags
emscripten_link_args = [
  '-s', 'ALLOW_MEMORY_GROWTH=1',
  '-s', 'USE_WEBGL2=1',
  '-s', 'FULL_ES3=1',
  '-s', 'WASM=1',
  '-s', 'NO_EXIT_RUNTIME=1',
  '-s', 'FETCH'
]

# Source files
sources = files(
  # Main
  'src/main.cpp',
  'src/main_loop.cpp',
  'src/mathlib.cpp',
  'src/logger.cpp',
  'src/shader.cpp',
  'src/shader_fetcher.cpp',
  'src/shapes_2d.cpp',
  'src/webgl_context.cpp',
  'src/renderer_2d.cpp',
  'src/renderer_3d.cpp',

  # Shaders
  'src/shaders/renderer2d_frag.cpp',
  'src/shaders/renderer2d_vert.cpp',
  'src/shaders/renderer3d_frag.cpp',
  'src/shaders/renderer3d_vert.cpp',
  'src/shaders/sun_frag.cpp',
  'src/shaders/sun_vert.cpp',

  # Autumn theme
  'src/autumn/autumn_theme.cpp',
  'src/autumn/leaf_particle_render.cpp',
  'src/autumn/tree_shape.cpp',

  # Winter theme
  'src/winter/winter_theme.cpp',
  'src/winter/snowflake.cpp',
  'src/winter/windfield.cpp',

  # Spring theme
  'src/spring/spring_theme.cpp',
  'src/spring/grass_renderer.cpp',

  # Summer theme
  'src/summer/summer_theme.cpp'
)

# Include directories
inc = include_directories(
  'src',
  'src/autumn',
  'src/winter',
  'src/spring',
  'src/summer',
  'src/shaders',
  'src/_shaders',
  'src/tools'
)

# Build the executable
output_exe = executable('output',
  sources,
  include_directories: inc,
  link_args: emscripten_link_args,
  name_suffix: 'js'
)

# Custom target to copy output files to dist directory
copy_script = find_program('sh')
copy_to_dist = custom_target('copy_to_dist',
  output: ['copy_to_dist.stamp'],
  input: output_exe,
  command: [
    copy_script, '-c',
    'mkdir -p ' + meson.project_source_root() + '/dist && ' +
    'cp $1 ' + meson.project_source_root() + '/dist/output.js && ' +
    'cp $2/output.wasm ' + meson.project_source_root() + '/dist/output.wasm && ' +
    'touch $3',
    '--', '@INPUT@', meson.current_build_dir(), '@OUTPUT@'
  ],
  build_by_default: true,
  install: false
)

# Add an alias target for convenience
alias_target('copy-dist', copy_to_dist)
