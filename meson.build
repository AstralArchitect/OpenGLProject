project('learnOpenGL', 'c', 'cpp', 'rust')

if host_machine.system() == 'windows'
  if run_command('powershell', 'Test-Path', '-Path', '".\glfw3.dll"', check: false).stdout() == 'False\n'
    message('Génération de glfw3.dll...')
    run_command('.\compile_glfw3.4.bat', check: false)
  endif
endif

if host_machine.system() == 'linux'

  if run_command('test', '-e', 'libglfw.so.3', check: false).returncode() == 1
    message('Génération de libglfw.so...')
    run_command('./compile_glfw3.4.sh', check: false)
  endif

endif



add_global_arguments('-I../Include/', language: ['cpp', 'c'])

if host_machine.system() == 'windows'
  add_global_link_arguments('-L../', '-lglfw3', '-Wl,-rpath=./', language: ['cpp', 'c'])
elif host_machine.system() == 'linux'
  add_global_link_arguments('-L../', '-lglfw', '-Wl,-rpath=./', language: ['cpp', 'c'])
endif


executable('main', 'main.cpp', 'lib/glad.c', 'lib/functions.cpp')