CFLAGS = -Wall -Wextra -pedantic -Werror

Game: main.o player.o updater.o NPC.o rooms.o items.o world.o GameLoop.o
	g++ -o Game main.o player.o NPC.o rooms.o items.o world.o GameLoop.o updater.o $(CFLAGS)

main.o: main.cpp
	g++ -c main.cpp $(CFLAGS)

player.o: player.cpp player.h
	g++ -c player.cpp $(CFLAGS)

NPC.o: NPC.cpp Entity.h player.h
	g++ -c NPC.cpp $(CFLAGS)

rooms.o: rooms.cpp rooms.h items.h Entity.h
	g++ -c rooms.cpp $(CFLAGS)

items.o: items.cpp
	g++ -c items.cpp $(CFLAGS)

world.o: world.cpp
	g++ -c world.cpp $(CFLAGS)

updater.o: updater.cpp
	g++ -c updater.cpp $(CFLAGS)

GameLoop.o: GameLoop.cpp
	g++ -c GameLoop.cpp $(CFLAGS)

clean:
	rm -f Game main.o player.o NPC.o rooms.o items.o world.o gameLoop.o updater.o