project('vgmsplit', 'c', 'cpp',
    version: '3.2.0',
    default_options: ['cpp_std=c++2a']
)


# TODO force as static library
libgme_proj = subproject('game-music-emu')
libgme_dep = libgme_proj.get_variable('libgme_dep')

executable('vgmsplit',
    'vgmsplit.cpp',
    'wave_writer.c',
    # Somehow, default_options cpp_std doesn't work.
    override_options : ['cpp_std=c++2a'],
    dependencies: libgme_dep,
    cpp_args: ['-DVGMSPLIT_VERSION="' + meson.project_version() + '"', '-DTEST=TEST'],
)
