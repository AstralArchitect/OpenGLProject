# Target to compile the project
all:
	meson compile -C builddir/

# Target to run the compiled binary
run: all
	./builddir/main

# Target to set up the build directory
setupGcc:
	meson setup builddir --native-file=native-gcc.txt
setup:
	meson setup builddir
# Target to clean the build directory
clean:
ifeq ($(OS),Windows_NT)
	@if exist builddir rmdir /S /Q builddir
	@if exist glfw3.dll del glfw3.dll
else
	@if [ -d builddir ]; then rm -rf builddir; fi
	@if [ -e libglfw.so ] || [ -e libglfw.so.3 ]; then rm libglfw.so*; fi
endif

