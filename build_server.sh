clear
inc_dir=../inc
lib_dir=../lib
bin_dir=../bin
src_dir=../src
compiler_flags="-std=c99 -ggdb -Wl,-rpath=$lib_dir -Wl,-rpath=$bin_dir -Wall -Wformat -D_DEBUG=1 -Wno-unused-function" 
mkdir bin -p
cd bin

gcc $compiler_flags -o $bin_dir/lib_network.so $src_dir/linux/network.c -shared -fPIC

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

gcc $compiler_flags -o $bin_dir/lib_server.so $src_dir/game/server.c -shared -fPIC \
-I$inc_dir \
-L$lib_dir -L$bin_dir \
-l:lib_network.so

rm -f distributed_pong
gcc $compiler_flags -o pong_server $src_dir/linux/pong_server.c -lm -ldl \
-I$inc_dir \
-L$lib_dir -L$bin_dir \
-l:lib_window.so -l:lib_server.so

#./pong_server 60001