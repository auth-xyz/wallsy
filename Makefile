# g++ src/main.cpp libs/wallsy.cpp -lclicky -std=c++20 -o wallsy
#

src = src/main.cpp
libs = libs/wallsy.cpp

all: $(src)
	g++ $(src) $(libs) -lclicky -std=c++20 -o wallsy

clean:
	rm -f wallsy

.PHONY: all clean
