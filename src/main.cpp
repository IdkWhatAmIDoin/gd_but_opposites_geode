#include <Geode/Geode.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <unordered_map>
#include <string>

using namespace geode::prelude;

// Map by ID
static const std::unordered_map<int, std::string> OPPOSITES_BY_ID = {
    {1, "Mono Sanity"},             // Stereo Madness
    {2, "Back off Track"},          // Back on Track
    {3, "Solarhuman"},              // Polargeist
    {4, "Wet In"},                  // Dry Out
    {5, "Peak After Peak"},         // Base After Base
    {6, "easiest level in gd :sob:"}, // Can't Let Go
    {7, "Sitter"},                  // Jumper
    {8, "Space Stationary"},        // Time Machine
    {9, "Static"},                  // Cycles
    {10, "yJump"},                  // xStep
    {11, "Cleanjazz"},              // Clutterfunk
    {12, "Fact of Nothing"},        // Theory of Everything
    {13, "Magnetboy Boringness"},   // Electroman Adventures
    {14, "Barstay"},                // Clubstep
    {15, "Magnetostatics"},         // Electrodynamix
    {16, "Circle Weakness"},        // Hexagon Force
    {17, "Drip Idle"},              // Blast Processing
    {18, "Fact of Nothing F"},      // Theory of Everything 2
    {19, "Algebraic Submissive"},   // Geometrical Dominator
    {20, "Aliveopened"},            // Deadlocked
    {21, "Toe Stay"},               // Fingerdash
    {22, "Stay"}                    // Dash
};

// Map by name (fallback)
static const std::unordered_map<std::string, std::string> OPPOSITES_BY_NAME = {
    {"Stereo Madness", "Mono Sanity"},
    {"Back on Track", "Back off Track"},
    {"Polargeist", "Solarhuman"},
    {"Dry Out", "Wet In"},
    {"Base After Base", "Peak After Peak"},
    {"Can't Let Go", "easiest level in gd :sob:"},
    {"Jumper", "Sitter"},
    {"Time Machine", "Space Stationary"},
    {"Cycles", "Static"},
    {"xStep", "yJump"},
    {"Clutterfunk", "Cleanjazz"},
    {"Theory of Everything", "Fact of Nothing"},
    {"Electroman Adventures", "Magnetboy Boringness"},
    {"Clubstep", "Barstay"},
    {"Electrodynamix", "Magnetostatics"},
    {"Hexagon Force", "Circle Weakness"},
    {"Blast Processing", "Drip Idle"},
    {"Theory of Everything 2", "Fact of Nothing F"},
    {"Geometrical Dominator", "Algebraic Submissive"},
    {"Deadlocked", "Aliveopened"},
    {"Fingerdash", "Toe Stay"},
    {"Dash", "Stay"}
};

static std::string getOppositeName(GJGameLevel* level) {
    if (!level) return "";
    
    // Try matching by ID first
    if (OPPOSITES_BY_ID.contains(level->m_levelID)) {
        return OPPOSITES_BY_ID.at(level->m_levelID);
    }
    
    // Fallback to matching by Name
    std::string currentName = level->m_levelName;
    if (OPPOSITES_BY_NAME.contains(currentName)) {
        return OPPOSITES_BY_NAME.at(currentName);
    }
    
    return "";
}

static GJGameLevel* modifyLevel(GJGameLevel* level) {
    if (!level) return level;
    
    std::string newName = getOppositeName(level);
    if (!newName.empty()) {
        level->m_levelName = newName;
    }
    
    return level;
}

$on_mod(Loaded) {
    log::info("Opposite Dash: Mod loaded");
}

class $modify(MyGameLevelManager, GameLevelManager) {
    GJGameLevel* getMainLevel(int levelID, bool dontGetLevelString) {
        auto level = GameLevelManager::getMainLevel(levelID, dontGetLevelString);
        return modifyLevel(level);
    }
};

class $modify(MyLevelTools, LevelTools) {
    static GJGameLevel* getLevel(int levelID, bool dontGetLevelString) {
        auto level = LevelTools::getLevel(levelID, dontGetLevelString);
        return modifyLevel(level);
    }
};

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        return true;
    }
};

class $modify(MyLevelSelectLayer, LevelSelectLayer) {
    bool init(int page) {
        return LevelSelectLayer::init(page);
    }
};

class $modify(MyLevelPage, LevelPage) {
    static void onModify(auto& self) {
        self.setHookPriority("LevelPage::init", -1000);
    }

    bool init(GJGameLevel* level) {
        // Ensure the level is modified before init
        level = modifyLevel(level);

        if (!LevelPage::init(level)) return false;
        if (!level) return true;

        std::string newName = getOppositeName(level);
        if (!newName.empty()) {
            Loader::get()->queueInMainThread([this, level, newName]() {
                // Redundant label update to be absolutely sure
                CCLabelBMFont* label = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("level-name-label"));
                if (!label) label = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("level-name"));
                if (!label) label = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("title-label"));

                if (!label) {
                    if (auto menu = this->getChildByID("level-menu")) {
                        if (auto button = menu->getChildByID("level-button")) {
                            auto children = button->getChildren();
                            if (children && children->count() > 0) {
                                if (auto whiteSprite = typeinfo_cast<CCNode*>(children->objectAtIndex(0))) {
                                    auto wsChildren = whiteSprite->getChildren();
                                    if (wsChildren && wsChildren->count() > 0) {
                                        if (auto scale9 = typeinfo_cast<CCNode*>(wsChildren->objectAtIndex(0))) {
                                            auto s9Children = scale9->getChildren();
                                            if (s9Children && s9Children->count() > 1) {
                                                label = typeinfo_cast<CCLabelBMFont*>(s9Children->objectAtIndex(1));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (label) {
                    label->setString(newName.c_str());
                }
            });
        }

        return true;
    }
};
