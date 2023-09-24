TARGET = exe
CC = g++
LIBS = -lm -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI
INC = ./include
SRC = ./src
CFLAGS = -g -Wall -fPIC -std=c++20 -m128bit-long-double -fext-numeric-literals -ffast-math -funroll-loops -I$(INC) -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_CHANNEL_MASK_DEPTH=32 -DMAGICKCORE_QUANTUM_DEPTH=16 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_CHANNEL_MASK_DEPTH=32 -DMAGICKCORE_QUANTUM_DEPTH=16 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_CHANNEL_MASK_DEPTH=32 -DMAGICKCORE_QUANTUM_DEPTH=16 -I/usr/local/include/ImageMagick-7
.PHONY: clean

DEPS = $(wildcard $(INC)/*.hpp)
OBJS = $(patsubst %.cpp, %.o, $(wildcard $(SRC)/*.cpp))

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	-rm -r $(SRC)/*.o
	-rm -r $(TARGET)