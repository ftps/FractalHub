TARGET = exe
CC = g++
NVCC = nvcc
LIBS = -lm -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI
INC = ./include
SRC = ./src
CXXFLAGS = -g -O0 -Wall -fPIC -std=c++20 -m128bit-long-double -fext-numeric-literals -ffast-math -funroll-loops -I$(INC) -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_CHANNEL_MASK_DEPTH=32 -DMAGICKCORE_QUANTUM_DEPTH=16 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_CHANNEL_MASK_DEPTH=32 -DMAGICKCORE_QUANTUM_DEPTH=16 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_CHANNEL_MASK_DEPTH=32 -DMAGICKCORE_QUANTUM_DEPTH=16 -I/usr/local/include/ImageMagick-7
CUDAFLAG = -c -arch=sm_75
.PHONY: clean

DEPS = $(wildcard $(INC)/*.hpp)
OBJS = $(patsubst %.cpp, %.o, $(wildcard $(SRC)/*.cpp)) $(patsubst %.cu, %.o, $(wildcard $(SRC)/*.cu))

%.o: %.cu $(DEPS)
	$(NVCC) $(CUDAFLAG) -c $< -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	-rm -r $(SRC)/*.o
	-rm -r $(TARGET)