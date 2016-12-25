GPP = g++
OUTFILE = "pawpy.so"
SDK_DIR = "../samp-plugin-sdk"
PYTHON_LIB = /home/southclaw/Python-3.6.0
COMPILE_FLAGS = -fpermissive -fPIC -m64 -std=c++11 -c -O3 -w -D LINUX -I$(SDK_DIR) -I$(SDK_DIR)/amx -I$(PYTHON_LIB) -I$(PYTHON_LIB)/Include

# python3.x-config --cflags and --ldflags
PYTHON_CFLAGS = -I/home/southclaw/include/python3.6m -I/home/southclaw/include/python3.6m  -Wno-unused-result -Wsign-compare -m32 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes
PYTHON_LDFLAGS = -L/home/southclaw/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/home/southclaw/lib -lpython3.6m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic

all: build

clean:
	-rm *~ *.o *.so

build:
	$(GPP) $(COMPILE_FLAGS) $(PYTHON_CFLAGS) $(SDK_DIR)/*.cpp
	$(GPP) $(COMPILE_FLAGS) $(PYTHON_CFLAGS) Pawpy/*.cpp
	$(GPP) $(PYTHON_LDFLAGS) -O2 -m32 -fshort-wchar -shared -o $(OUTFILE) *.o
