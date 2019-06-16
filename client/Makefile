CC := g++
SOURCE := client.cpp 
EXE := client

all: build

build: $(EXE)

$(EXE): $(SOURCE)
	@$(CC) $^ -o $@ -lcurl

clean:
	rm -rf $(EXE)
