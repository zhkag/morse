from building import *

cwd     = GetCurrentDir()
src     = Glob('src/morse.c')
CPPPATH    = [cwd + '/inc']
    
if GetDepend(['PKG_USING_MORSE_SHELL']):
	src += Split("""
	src/morse_shell.c
	""")

if GetDepend(['PKG_USING_MORSE_SAMPLE']):
	if GetDepend(['PKG_USING_MORSE_KEY_SAMPLE']):
		src += Split("""
		samples/morse-key-example.c
		""")
	if GetDepend(['PKG_USING_MORSE_TPAD_SAMPLE']):
		CPPPATH += [cwd + '/samples/tpad']
		src += Split("""
		samples/tpad/tpad.c
		samples/tpad/morse-tpad-example.c
		""")

group = DefineGroup('morse', src, depend = ['PKG_USING_MORSE'], CPPPATH = CPPPATH)

Return('group')
