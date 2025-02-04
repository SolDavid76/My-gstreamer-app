CC = g++
CFLAGS = -Wall -Wextra -std=c++11 `pkg-config --cflags gstreamer-1.0`
LDFLAGS = `pkg-config --libs gstreamer-1.0`
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = GStreamerApp

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
