project('vgmsplit', 'cpp',
    version: '3.2.0',
    default_options: ['cpp_std=c++2a', 'default_library=static'],
)

# When you change default_options, meson prints the discrepancy
# but does not automatically apply the changes.
# The easiest way I've found is to delete and recreate the build directory.
# The official documentation is at https://mesonbuild.com/Configuring-a-build-directory.html .

libgme_proj = subproject('game-music-emu')
libgme_dep = libgme_proj.get_variable('libgme_dep')

executable('vgmsplit',
    'src/main.cpp',
    'src/renderer.cpp',
    'src/wave_writer.cpp',
    # Somehow, default_options cpp_std doesn't work.
    override_options : ['cpp_std=c++2a'],
    dependencies: libgme_dep,
    cpp_args: ['-DVGMSPLIT_VERSION="' + meson.project_version() + '"', '-DTEST=TEST'],
)
