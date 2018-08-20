# g++ \
#     towave.cpp -Wl,-Bstatic wave_writer.c -L/home/nyanpasu64/src/game-music-emu/build/gme/ -l:libgme.so \
#     || exit
# 

g++ towave.cpp wave_writer.c \
-L/home/nyanpasu64/src/game-music-emu/build/gme/ -Wl,-rpath . -lgme \
|| exit

ls -lhd a.out
