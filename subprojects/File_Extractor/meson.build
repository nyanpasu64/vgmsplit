project('fex', 'cpp')

inc = include_directories('fex')
src = files('fex/Data_Reader.cpp')

# zlib is used to load .vgz files, and maybe others.
zlib_dep = dependency('zlib')

fex = library('fex',
    src,
    cpp_args: ['-DNDEBUG', '-DHAVE_STDINT_H', '-DHAVE_ZLIB_H'],
    include_directories: [inc],
    dependencies: [zlib_dep],
)

fex_dep = declare_dependency(
    compile_args: ['-DHAVE_STDINT_H', '-DHAVE_ZLIB_H'],
    include_directories: [inc],
    link_with: fex,
)
