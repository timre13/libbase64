main: main.cpp
	g++ main.cpp -Wall -Wextra -Wpedantic -o main
	#g++ main.cpp -Wall -Wextra -Wpedantic -o main -fsanitize=address,leak,undefined

clean:
	rm main
