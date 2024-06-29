# Target to compile the project
all:
	meson compile -C builddir/

# Target to run the compiled binary
run: all
ifeq ($(OS),Windows_NT)
	.\addToPathAndRun.bat
else
	./builddir/mainR
endif

# Target to set up the build directory
setupGcc:
	meson setup builddir --native-file=native-gcc.txt
setup:
	meson setup builddir
ifeq ($(OS),Windows_NT)
	@if exist builddir/build.ninja (powershell -NoLogo -NoProfile -Command "(Get-Content builddir\build.ninja) -replace ' ""msvcrt.lib""""', '' | Set-Content builddir\build.ninja")
endif

# Target to clean the build directory
clean:
ifeq ($(OS),Windows_NT)
	@if exist builddir (rmdir builddir /S /Q)
else
	@if [ -d builddir ]; then rm -rf builddir; fi
endif

resetup: clean setup
resetupGcc: clean setupGcc