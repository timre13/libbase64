main: main.cpp
	g++ main.cpp src/Base64.cpp -Wall -Wextra -Wpedantic -o main
	#g++ main.cpp src/Base64.cpp -Wall -Wextra -Wpedantic -o main -fsanitize=address,leak,undefined

clean:
	rm main
