src/edit.cpp:
	g++ src/edit.cpp -lncurses -Wunused-value -o build/edit && build/edit README.md

clean:
	rm build/edit
