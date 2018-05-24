clear
inc_dir=../inc
lib_dir=../lib
bin_dir=../bin
src_dir=../src
compiler_flags="-std=c99 -ggdb -Wl,-rpath=$lib_dir -Wl,-rpath=$bin_dir -Wall -Wformat -D_DEBUG=1"
mkdir bin -p
cd bin

gcc $compiler_flags -o $bin_dir/lib_window.so $src_dir/linux/window.c -shared -fPIC \
-I$inc_dir -I$inc_dir/gl3w \
-L$lib_dir -L$bin_dir \
-lGL \
`pkg-config --cflags glfw3` `pkg-config --static --libs glfw3`

gcc $compiler_flags -o $bin_dir/lib_ui.so $src_dir/linux/ui.c -shared -fPIC \
-I$inc_dir -I$inc_dir/gl3w \
-L$lib_dir -L$bin_dir \
-l:cimgui.so -lGL \
`pkg-config --cflags glfw3` `pkg-config --static --libs glfw3`

gcc $compiler_flags -o $bin_dir/lib_rendering.so $src_dir/linux/rendering.c -shared -fPIC \
-I$inc_dir -I$inc_dir/gl3w \
-L$lib_dir -L$bin_dir \
-lGL \
`pkg-config --cflags glfw3` `pkg-config --static --libs glfw3`

gcc $compiler_flags -o $bin_dir/lib_input.so $src_dir/linux/input.c -shared -fPIC \
-I$inc_dir -I$inc_dir/gl3w \
-L$lib_dir -L$bin_dir \
-l:cimgui.so -lGL \
`pkg-config --cflags glfw3` `pkg-config --static --libs glfw3`

gcc $compiler_flags -o $bin_dir/lib_game.so $src_dir/game/game.c -shared -fPIC \
-I$inc_dir \
-L$lib_dir -L$bin_dir \
-l:lib_ui.so -l:lib_input.so -l:lib_rendering.so

rm -f distributed_pong
gcc $compiler_flags -o distributed_pong $src_dir/linux/main.c -lm -ldl \
-I$inc_dir \
-L$lib_dir -L$bin_dir \
-l:lib_window.so

./distributed_pong