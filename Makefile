edit.cpp:
	g++ edit.cpp -lncurses -Wunused-value -o edit && ./edit README.md

clean:
	rm edit
