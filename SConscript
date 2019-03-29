Import('RTT_ROOT')
Import('rtconfig')
from building import *

cwd = GetCurrentDir()

objs = []

list = os.listdir(cwd)

for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        objs = objs + SConscript(os.path.join(d, 'SConscript'))

src = Glob('*.c')
CPPPATH = [cwd, str(Dir('#'))]

#path  = [cwd + '/code/beep', cwd + '/code/button', cwd + '/code/decode', cwd + '/code/key', cwd + '/code/led', cwd + '/code/player', cwd + '/samples']

#CPPPATH = [cwd, str(Dir('#')), cwd + '/rtt_inc']
#CPPPATH = [cwd, cwd + '/syscontrol']

group = DefineGroup('Applications', src, depend = [''], CPPPATH = CPPPATH)

Return('group')

# Import('RTT_ROOT')
# Import('rtconfig')
# from building import *

# cwd = GetCurrentDir()
# src = Glob('*.c')
# CPPPATH = [cwd, str(Dir('#'))]

# group = DefineGroup('Applications', src, depend = [''], CPPPATH = CPPPATH)

# Return('group')
