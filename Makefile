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
	@if exist builddir (rmdir /S /Q builddir)
else
	@if exist builddir (rm -rf builddir/)
endif
