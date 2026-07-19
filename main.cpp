#include "world.h"
#include "rooms.h"
#include "save.h"
#include "updater.h"
#include "GameLoop.h"
#include "player.h"
#include "items.h"
#include "Entity.h"
#include "stats.h"

int main() {
    Stats startStats = {5, 5, 5, 5};
    Player player("Thomas", "A runner with no memory.", 100.0, startStats);

    World world;
    world.createWorld();

    player.setLocation(world.getRoomByName("The Cage"));

    GameLoop loop(world, player);
    loop.run();

    return 0;
}