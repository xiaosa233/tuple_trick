import os
import subprocess
import sys

workspace = os.path.dirname(os.path.abspath(__file__))

cmd = [sys.executable, os.path.join(workspace, '../quick_cmake/sources/quick_cmake/main.py'), '--workspace', workspace,
     '--std=c++17']

subprocess.run(cmd)
