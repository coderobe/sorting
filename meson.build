project('sorting', 'cpp',
  default_options: [
    'cpp_std=c++17',
  ],
)

if get_option('buildtype').startswith('debug')
  add_project_arguments(
    '-Og',
    '-g',
    '-fsanitize=address',
    '-fno-omit-frame-pointer',
    language: 'cpp',
  )
  add_project_link_arguments(
    '-fsanitize=address',
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

if host_machine.system() == 'darwin'
  dependencies += dependency('Foundation')
endif

exe = executable(
  'sorting',
  sources: sources,
  dependencies: dependencies,
  gui_app: true,
)
