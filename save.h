#ifndef SAVE_H
#define SAVE_H

#include <string>

class Player;
class World;

class SaveManager {
public:
    static bool saveGame(const std::string& filename, Player& player, World& world);
    static bool loadGame(const std::string& filename, Player& player, World& world);

private:
    static std::string escapeJson(const std::string& s);
};

#endif // SAVE_H