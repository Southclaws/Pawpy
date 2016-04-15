import shutil
import os
import sys
path = sys.argv[1]
shutil.copyfile("../%s/Pawpy.dll"%(path), "E:/Games/Projects/SA-MP/Blank/plugins/Pawpy.dll")
os.chdir("E:/Games/Projects/SA-MP/Blank/")
os.system("E:/Games/Projects/SA-MP/Blank/samp-server.exe")
