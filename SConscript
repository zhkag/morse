from building import *

cwd     = GetCurrentDir()
src     = Glob('src/*.c')
CPPPATH    = [cwd + '/inc']

if GetDepend(['PKG_USING_MORSE_KEY_SAMPLE']):
    src += Split("""
    samples/morse-key-example.c
    """)

group = DefineGroup('morse', src, depend = ['PKG_USING_MORSE'], CPPPATH = CPPPATH)

Return('group')
