CC := g++ -std=c++11
SOURCE := server.cpp 
EXE := server

all: build

build: $(EXE)

$(EXE): $(SOURCE)
	@$(CC) $^ -o $@ -levent

clean:
	rm -rf $(EXE)
