project('vgmsplit', 'c', 'cpp',
    version: '3.2.0',
)

# TODO force as static library
libgme_proj = subproject('game-music-emu')
libgme_dep = libgme_proj.get_variable('libgme_dep')

executable('vgmsplit',
    'vgmsplit.cpp',
    'wave_writer.c',
    dependencies: libgme_dep,
    cpp_args: ['-DVGMSPLIT_VERSION=' + meson.project_version(), '-DTEST=TEST'],
)
