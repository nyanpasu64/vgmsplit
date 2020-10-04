project('vgmsplit', 'c', 'cpp',
    version: '3.2.0',
)

executable('vgmsplit',
    'vgmsplit.cpp',
    'wave_writer.c',
    include_directories: ['game-music-emu'],
    cpp_args: ['-DVGMSPLIT_VERSION=' + meson.project_version(), '-DTEST=TEST'],
)
