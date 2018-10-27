project('sorting', 'cpp',
  default_options: [
    'cpp_std=c++17',
  ],
)

add_project_arguments(
  '-Wno-sizeof-pointer-div',
  language: 'cpp',
)

if get_option('buildtype').startswith('debug')
  add_project_arguments(
    '-O0',
    '-g',
#    '-fsanitize=address',
    '-fno-omit-frame-pointer',
    language: 'cpp',
  )
  add_project_link_arguments(
#    '-fsanitize=address',
    language: 'cpp',
  )
else
  add_project_arguments(
    '-march=x86-64',
    '-mtune=generic',
    '-pipe',
    '-fno-plt',
    '-Os',
    '-ffast-math',
    '-fno-math-errno',
    '-fno-ident',
    '-ffunction-sections',
    '-fdata-sections',
    '-fmerge-all-constants',
    '-gsplit-dwarf',
    language: 'cpp',
  )
endif

add_project_link_arguments(
  language: 'cpp',
)

glob = run_command('meson/wildcard', 'src/**/*.cpp')
sources = glob.stdout().strip().split('\n')

dependencies = [
  dependency('threads'),
  dependency('glfw3'),
  dependency('GL'),
  dependency('glew'),
]

exe = executable(
  'sorting',
  sources: sources,
  dependencies: dependencies,
  gui_app: true,
)
