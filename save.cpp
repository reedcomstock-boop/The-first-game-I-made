#include "save.h"
#include "player.h"
#include "world.h"
#include "rooms.h"
#include "stats.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string SaveManager::escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

bool SaveManager::saveGame(const std::string& filename, Player& player, World& world) {
    std::ofstream out(filename);
    if (!out) {
        std::cout << "Could not open '" << filename << "' for saving.\n";
        return false;
    }

    Room* loc = player.getLocation();
    Stats s = player.getStats();

    out << "{\n";
    out << "  \"name\": \"" << escapeJson(player.getName()) << "\",\n";
    out << "  \"description\": \"" << escapeJson(player.getDescription()) << "\",\n";
    out << "  \"health\": " << player.getHealth() << ",\n";
    out << "  \"energy\": " << player.getEnergy() << ",\n";
    out << "  \"exp\": " << player.getExp() << ",\n";
    out << "  \"expToNextLevel\": " << player.getExpToNextLevel() << ",\n";
    out << "  \"magic\": " << (player.hasMagic() ? "true" : "false") << ",\n";
    out << "  \"inCombat\": " << (player.getInCombat() ? "true" : "false") << ",\n";
    out << "  \"dialogueProgress\": " << player.getDiologueProgress() << ",\n";
    out << "  \"location\": \"" << escapeJson(loc ? loc->getName() : "") << "\",\n";
    out << "  \"worldLevel\": " << world.getWorldLevel() << ",\n";
    out << "  \"stats\": {\n";
    out << "    \"strength\": "     << s.strength     << ",\n";
    out << "    \"dexterity\": "    << s.dexterity    << ",\n";
    out << "    \"intelligence\": " << s.intelligence << ",\n";
    out << "    \"defence\": "      << s.defence      << "\n";
    out << "  },\n";

    out << "  \"inventory\": [\n";
    const auto& items = player.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        Tool* tool = dynamic_cast<Tool*>(items[i]);
        out << "    { \"name\": \"" << escapeJson(items[i]->getName()) << "\", ";
        out << "\"description\": \"" << escapeJson(items[i]->getDescription()) << "\", ";
        if (tool) {
            out << "\"isTool\": true, ";
            out << "\"level\": " << tool->getLevel() << ", ";
            out << "\"toolHealth\": " << tool->getHealth() << ", ";
            out << "\"toolEnergy\": " << tool->getEnergy() << ", ";
            const Stats& ts = tool->getStats();
            out << "\"toolStats\": {\"strength\": " << ts.strength
                << ", \"dexterity\": " << ts.dexterity
                << ", \"intelligence\": " << ts.intelligence
                << ", \"defence\": " << ts.defence << "}";
        } else {
            out << "\"isTool\": false";
        }
        out << " }" << (i + 1 < items.size() ? ",\n" : "\n");
    }
    out << "  ]\n";
    out << "}\n";

    out.close();
    std::cout << "Game saved to '" << filename << "'.\n";
    return true;
}
namespace {
    // Skips whitespace
    void skipWs(const std::string& s, size_t& i) {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    }

    // Reads a JSON string literal starting at s[i] == '"'
    std::string readString(const std::string& s, size_t& i) {
        std::string out;
        ++i; // skip opening quote
        while (i < s.size() && s[i] != '"') {
            if (s[i] == '\\' && i + 1 < s.size()) {
                out += s[i + 1];
                i += 2;
            } else {
                out += s[i];
                ++i;
            }
        }
        ++i; // skip closing quote
        return out;
    }

    double readNumber(const std::string& s, size_t& i) {
        size_t start = i;
        while (i < s.size() && (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '-' || s[i] == '.' )) ++i;
        return std::stod(s.substr(start, i - start));
    }

    bool readBool(const std::string& s, size_t& i) {
        if (s.compare(i, 4, "true") == 0) { i += 4; return true; }
        i += 5; // "false"
        return false;
    }

    // Finds "key": value and returns index just after the colon
    size_t findKey(const std::string& s, const std::string& key, size_t from = 0) {
        std::string pattern = "\"" + key + "\"";
        size_t pos = s.find(pattern, from);
        if (pos == std::string::npos) return std::string::npos;
        pos = s.find(':', pos);
        if (pos == std::string::npos) return std::string::npos;
        return pos + 1;
    }
}

bool SaveManager::loadGame(const std::string& filename, Player& player, World& world) {
    std::ifstream in(filename);
    if (!in) {
        std::cout << "Could not open '" << filename << "' for loading.\n";
        return false;
    }

    std::stringstream buf;
    buf << in.rdbuf();
    std::string s = buf.str();
    in.close();

    size_t i;

    if ((i = findKey(s, "name")) != std::string::npos) {
        skipWs(s, i);
        player.setName(readString(s, i));
    }
    if ((i = findKey(s, "description")) != std::string::npos) {
        skipWs(s, i);
        player.setDescription(readString(s, i));
    }
    if ((i = findKey(s, "health")) != std::string::npos) {
        skipWs(s, i);
        player.setHealth(readNumber(s, i));
    }
    if ((i = findKey(s, "energy")) != std::string::npos) {
        skipWs(s, i);
        player.setEnergy(readNumber(s, i));
    }
    if ((i = findKey(s, "exp")) != std::string::npos) {
        skipWs(s, i);
        player.setExp(readNumber(s, i));   // note: setExp ADDS, see caveat below
    }
    if ((i = findKey(s, "magic")) != std::string::npos) {
        skipWs(s, i);
        player.setMagic(readBool(s, i));
    }
    if ((i = findKey(s, "inCombat")) != std::string::npos) {
        skipWs(s, i);
        player.setInCombat(readBool(s, i));
    }
    if ((i = findKey(s, "dialogueProgress")) != std::string::npos) {
        skipWs(s, i);
        player.setDiologueProgress(static_cast<int32_t>(readNumber(s, i)));
    }
    if ((i = findKey(s, "worldLevel")) != std::string::npos) {
        skipWs(s, i);
        world.setWorldLevel(static_cast<int32_t>(readNumber(s, i)));
    }

    std::string locName;
    if ((i = findKey(s, "location")) != std::string::npos) {
        skipWs(s, i);
        locName = readString(s, i);
    }
    Room* loc = world.getRoomByName(locName);
    if (loc) player.setLocation(loc);

    std::cout << "Game loaded from '" << filename << "'.\n";
    return true;
}