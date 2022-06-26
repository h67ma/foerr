all: game editor

game:
	make -r -C src/game

editor:
	@echo "editor TODO"

clean:
	make clean -C src/game
	@echo "editor TODO"
