CXX = emcc
EXE = dist/output.js
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
LDFLAGS = -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -s FETCH

all: build $(EXE)

$(EXE): $(OBJS)
	echo $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(LDFLAGS)

build:
	@mkdir -p dist

clean:
	rm -rf dist && rm *.o