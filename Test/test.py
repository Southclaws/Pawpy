import configparser
import shutil
import os
import sys

# load samp server path to use as test
cfg = configparser.ConfigParser()
cfg.read("test.config")

print(cfg["test"]["samp_path"])
print(cfg["test"]["pawn_path"])

dll_path = "../" + sys.argv[1]
samp_path = cfg["test"]["samp_path"]
pawn_path = cfg["test"]["pawn_path"]

# copy the dll to the samp server plugins dir
shutil.copyfile("%s/Pawpy.dll"%(dll_path), "%s/plugins/Pawpy.dll"%(samp_path))

# compile the test gamemode
os.system(pawn_path + "pawncc.exe test.pwn -o\""+ samp_path + "/gamemodes/test.amx\"")

# run the samp server (must have gamemode0 set to "test" in server.cfg)
#os.chdir(samp_path)
#os.system("samp-server.exe")
