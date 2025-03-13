# Target to compile the project
all:
	meson compile -C builddir/

# Target to run the compiled binary
run: all
ifeq ($(OS),Windows_NT)
	.\addToPathAndRun.bat
else
	./builddir/main
endif

setup:
	meson setup builddir

# Target to clean the build directory
clean:
ifeq ($(OS),Windows_NT)
	@if exist builddir (rmdir builddir /S /Q)
else
	@if [ -d builddir ]; then rm -rf builddir; fi
endif

resetup: clean setup
resetupGcc: clean setupGcc