project('learnOpenGL', 'c', 'cpp', 'rust')

if host_machine.system() == 'windows'
  result = run_command('powershell', 'Test-Path', '-Path', '".\glfw3.dll"', check: false).stdout()
  if result == 'False\n'
    message('Génération de glfw3.dll...')
    run_command('.\compile_glfw3.4.bat', check: false)
  endif
endif

if host_machine.system() == 'linux'
  result = run_command('test', '-e', 'libglfw.so.3', check: false).returncode()
  if result == 1
    message('Génération de libglfw.so...')
    run_command('./compile_glfw3.4.sh', check: false)
  endif
endif



# Add include directory
add_global_arguments('-I../Include/', language: ['cpp', 'c'])

# Add library directory and link with GLFW
if host_machine.system() == 'windows'
  add_global_link_arguments('-L../', '-lglfw3', language: ['cpp', 'c'])
elif host_machine.system() == 'linux'
  add_global_link_arguments('-L../', '-lglfw', '-Wl,-rpath=./', language: ['cpp', 'c'])
endif
# Build the executable
executable('main', 'main.cpp', 'glad.c')