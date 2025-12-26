## tasks
-pedantic
-Wall
-Wextra
-Wcas
-align
-Wcas
-qual
-Wcto
-dto
-privacy
-Wdisable
-optimization
-Wformat=2
-Wini
-self
-Wlogica
-op
-Wmissin
-declarations
-Wmissin
-includ
-dirs
-Wnoexcept
-Wol
-styl
-cast
-Woverloade
-virtual
-Wredundan
-decls
-Wshadow
-Wsig
-conversion
-Wsig
-promo
-Wstric
-nul
-sentinel
-Wstric
-overflow=5
-Wswitc
-default
-Wundef
-Werror
-Wno-unused

- improve build time! it is unbearable!!

	- build as one translation unit
	- see what dependencies are bloating it out & replace
		- ncurses
		- string
		- vector
		- fstream
		- algorithm
		- climits
		- cstdint
		- map
		- mutex
	- compiler flags turn off exceptions
	- more compiler flags
	- remove make
	- performance indicators

- more tests

- editing new file indicator [NEW] on right side??

- repeat number keys

- W and B

- ts-server integration (do it for real this time)
