#ifndef GAMELOOP_H
#define GAMELOOP_H
#include <iostream>
#include <sstream>
#include <algorithm>
#include "world.h"
#include "player.h"
 
class GameLoop {
public:
    GameLoop(World& world, Player& player);
    void run();
 
private:
    World&  world;
    Player& player;
    bool playing;
 
    void printSituation() const;
    void showHelp() const;
 
    // Command handlers
    bool cmdGo(const std::string& direction);
    bool cmdPickup(const std::string& itemName);
    bool cmdDrop(const std::string& itemName);
    bool cmdInventory() const;
    bool cmdUseTool(const std::string& itemName);
    bool cmdEquip(const std::string& itemName);
    bool cmdUnequip(const std::string& itemName);
    bool cmdAttack(const std::string& targetName);
    bool cmdFlee();
    bool cmdUseMagic();
    bool cmdTalk(const std::string& npcName);
    bool cmdLook() const;
    bool cmdMe() const;
    bool cmdLoad(const std::string&filename);
    bool cmdSave(const std::string&filename);
    bool cmdCraft(const std::string& itemName);
    void checkWorldProgression();
};
#endif // GAMELOOP_H
 
