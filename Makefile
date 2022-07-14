main: main.cpp include/Base64.h src/Base64.cpp
	g++ main.cpp src/Base64.cpp -g3 -Wall -Wextra -Wpedantic -o main
	#g++ main.cpp src/Base64.cpp -g3 -Wall -Wextra -Wpedantic -o main -fsanitize=address,leak,undefined

clean:
	rm main
