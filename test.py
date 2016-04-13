import shutil
import os
shutil.copyfile("./Debug/Pawpy.dll", "E:/Games/Projects/SA-MP/Blank/plugins/Pawpy.dll")
os.chdir("E:/Games/Projects/SA-MP/Blank/")
os.system("E:/Games/Projects/SA-MP/Blank/samp-server.exe")
