import configparser
import shutil
import os
import sys

# load samp server path to use as test
cfg = configparser.ConfigParser()
cfg.read("test.config")

print(cfg["test"]["samp_path"])

dll_path = sys.argv[1]
samp_path = cfg["test"]["samp_path"]

shutil.copyfile("%s/Pawpy.dll"%(dll_path), "%s/plugins/Pawpy.dll"%(samp_path))
os.chdir(samp_path)
os.system("samp-server.exe")
