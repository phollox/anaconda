import sys
sys.path.append('../../../')
from cx_Freeze import setup, Executable

build_options = {'packages': ['PIL', 'mmfparser'],
                 'excludes': ['tcl', 'tk', 'Tkinter']}

executables = [
    Executable('roombuilder.py', base='Console', targetName='roombuilder.exe')
]

setup(options={'build_exe': build_options}, executables=executables)
