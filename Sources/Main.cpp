#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>
#include <vector>

#include "Codes.hpp"
#include "PKHeX.hpp"

namespace CTRPluginFramework {
    // This function is called before main and before the game starts. Useful to do code edits safely
    void PatchProcess(FwkSettings &settings) {
        DetectGame(Process::GetTitleID());
        IsUpdateSupported(Process::GetVersion());
    }

    // This function is called when the process exits. Useful to save settings, undo patchs or clean up things
    void OnProcessExit(void);

    void InitMenu(PluginMenu &menu) {
        menu += new MenuEntry("PSS", nullptr, PlayerSearchSystem); // <W
        MenuFolder *combat = new MenuFolder("Combat");
        *combat += new MenuEntry("Party Position:", nullptr, PartyPosition); // <W

        MenuFolder *inBattle = new MenuFolder("In-Battle", {
            new MenuEntry("Condition", nullptr, StatusCondition), // <W
            new MenuEntry("Stats", nullptr, Stats), // <W
            new MenuEntry("Health & Mana", nullptr, HealthAndMana), // <W
            new MenuEntry("Item", nullptr, HeldItem), // <W
            new MenuEntry("Moves", nullptr, Moves) // <W
        });

        *combat += inBattle;
        *combat += new MenuEntry("Exp. Multiplier", nullptr, ExpMultiplier); // <W
        *combat += HotkeyEntry(new MenuEntry("View Pokemon Infos.", nullptr, ViewPokemonInfo, ""), {Key::Start, ""}); // <W
        *combat += new MenuEntry("Allow 1+ Megas", AllowMultipleMegas); // <W

        MenuFolder *other = new MenuFolder("Other", {
            new MenuEntry("No Wild Pokemon", NoWildPokemon), // <W
            new MenuEntry("Wild Pokemon Spawner", nullptr, WildSpawner), // <W
            new MenuEntry("Always Shiny", AlwaysShiny), // <W
            new MenuEntry("Capture Rate 100%", CaptureRate), // <W
            new MenuEntry("Capture Trainer Pokemon", CatchTrainerPokemon) // <W
        });

        *combat += other;
        menu += combat;

        MenuFolder *pkhex = new MenuFolder("PKHeX");
        MenuFolder *trainer = new MenuFolder("Trainer");

        MenuFolder *info = new MenuFolder("Info", {
            new MenuEntry("TID & SID", nullptr, Identity), // <W
            new MenuEntry("Name", nullptr, InGameName), // <W
            new MenuEntry("Time Played", nullptr, PlayTime), // <W
            new MenuEntry("Game Language", nullptr, GameLanguage) // <W
        });

        *trainer += info;
        MenuFolder *bag = new MenuFolder("Bag");

        MenuFolder *currency = new MenuFolder("Currency", {
            new MenuEntry("Money", nullptr, Money), // <W
            new MenuEntry("Battle Points", nullptr, BattlePoints), // <W
            new MenuEntry("Poké Miles", nullptr, PokeMiles) // <W
        });

        *bag += currency;
        *bag += new MenuEntry("Items", nullptr, Items); // <W
        *bag += new MenuEntry("Medicines", nullptr, Medicines); // <W
        *bag += new MenuEntry("Berries", nullptr, Berries); // <W
        *bag += new MenuEntry("TMs & HMs", nullptr, KeyItems); // <W
        *trainer += bag;
        *pkhex += trainer;
        MenuFolder *box = new MenuFolder("PC Box");
        *box += new MenuEntry("Export & Import", nullptr, ExportImport); // <W
        MenuFolder *editor = new MenuFolder("Editor");
        *editor += new MenuEntry("Position", nullptr, PKHeX::Position); // <W

        MenuFolder *mainEditor = new MenuFolder("Main", {
            new MenuEntry("Shiny", nullptr, PKHeX::Shiny), // <W
            new MenuEntry("Species", nullptr, PKHeX::Species), // <W
            new MenuEntry("Is Nicknamed", nullptr, PKHeX::IsNicknamed), // <W
            new MenuEntry("Nickname", nullptr, PKHeX::Nickname), // <W
            new MenuEntry("Level", nullptr, PKHeX::Level), // <W
            new MenuEntry("Nature", nullptr, PKHeX::Nature), // <W
            new MenuEntry("Gender", nullptr, PKHeX::Gender), // <W
            new MenuEntry("Form", nullptr, PKHeX::Form), // <W
            new MenuEntry("Held Item", nullptr, PKHeX::HeldItem), // <W
            new MenuEntry("Ability", nullptr, PKHeX::Ability), // <W
            new MenuEntry("Friendship", nullptr, PKHeX::Friendship), // <W
            new MenuEntry("Language", nullptr, PKHeX::Language), // <W
            new MenuEntry("Is Egg", nullptr, PKHeX::IsEgg), // <W
            new MenuEntry("Pokérus", nullptr, PKHeX::Pokerus), // <W
            new MenuEntry("Country", nullptr, PKHeX::Country), // <W
            new MenuEntry("3DS Region", nullptr, PKHeX::ConsoleRegion) // <W
        });

        *editor += mainEditor;

        MenuFolder *origins = new MenuFolder("Origins", {
            new MenuEntry("Game From", nullptr, PKHeX::OriginGame), // < W
            new MenuEntry("Ball", nullptr, PKHeX::Ball), // <W
            new MenuEntry("Met Level", nullptr, PKHeX::MetLevel), // <W
            new MenuEntry("Met Date", nullptr, PKHeX::MetDate), // <W
            new MenuEntry("Is Fateful Enc.", nullptr, PKHeX::IsFatefulEncounter), // <W
            new MenuEntry("Egg Met Date", nullptr, PKHeX::EggMetDate) // <W
        });

        *editor += origins;

        MenuFolder *stats = new MenuFolder("Stats", {
            new MenuEntry("IV", nullptr, PKHeX::IV), // <W
            new MenuEntry("EV", nullptr, PKHeX::EV), // <W
            new MenuEntry("Contest", nullptr, PKHeX::Contest) // <W
        });

        *editor += stats;

        MenuFolder *moves = new MenuFolder("Moves", {
            new MenuEntry("Current Moves", nullptr, PKHeX::CurrentMove), // <W
            new MenuEntry("PP Ups", nullptr, PKHeX::PPUp), // <W
            new MenuEntry("Relearn Moves", nullptr, PKHeX::RelearnMove) // <W
        });

        *editor += moves;

        MenuFolder *handlerInfo = new MenuFolder("Misc.", {
            new MenuEntry("SID", nullptr, PKHeX::SID), // <W
            new MenuEntry("TID", nullptr, PKHeX::TID), // <W
            new MenuEntry("OT Name", nullptr, PKHeX::OTName), // <W
            new MenuEntry("Latest (not OT) Handler", nullptr, PKHeX::LatestHandler) // <W
        });

        *editor += handlerInfo;
        *box += editor;
        *pkhex += box;
        *pkhex += new MenuEntry("PC Anywhere", PCAnywhere); // <W
        *pkhex += new MenuEntry("31 Boxes Unlocked", nullptr, BoxesUnlocked); // <W
        menu += pkhex;

        MenuFolder *misc = new MenuFolder("Misc.");
        *misc += new MenuEntry("Fast Walk & Run", FastWalkRun); // <W
        *misc += new MenuEntry("Walk Through Walls", WalkThroughWalls); // <W
        *misc += new MenuEntry("Stay in Action Mode", StayInAction); // <W
        *misc += new MenuEntry("Unlock Full Fly Map", nullptr, UnlockFullFlyMap); // <W

        MenuFolder *random = new MenuFolder("Other", {
            new MenuEntry("Rename Pokemon", RenameAnyPokemon), // <W
            new MenuEntry("Learn Any TM & HM", LearnAnyTeachable), // <W
            new MenuEntry("Instant D.C Egg", InstantEgg), // <W
            new MenuEntry("Hatch Egg Immediately", InstantEggHatch) // <W
        });

        *misc += random;
        *misc += HotkeyEntry(new MenuEntry("View IVs & EVs in Summary", ViewValuesInSummary, ""), {Hotkey(Key::L, ""), Hotkey(Key::Start, "")}); // <W
        *misc += new MenuEntry("No Outlines", NoOutlines); // <W
        *misc += new MenuEntry("Super Fast Dialogs", FastDialogs); // <W
        menu += misc;
    }

    int main(void) {
        PluginMenu *menu = new PluginMenu("Gen 6 CTRPluginFramework", 0, 0, 1, "Gen 6 CTRPluginFramework is a 3gx plugin for Pokemon X, Y, Omega Ruby, and Alpha Sapphire on the Nintendo 3DS console. This plugin is based off of Alolan CTRPluginFramework, with PKHeX implementations in an updated .3gx file format.\n\nRepository:\ngithub.com/biometrix76");
        // Enable menu synchronization with the game's frame rate
        menu->SynchronizeWithFrame(true);
        // Pause the execution for 100 milliseconds to ensure the menu is properly initialized
        Sleep(Milliseconds(100));

        // Check if the current game is recognized
        if (currGameName == GameName::None || currGameSeries == GameSeries::None)
            Process::ReturnToHomeMenu(); // If not recognized, return to the home menu of the console

        // Initialize the menu with entries and folders
        InitMenu(*menu);
        // Set the battle offset callback for the menu
        menu->Callback(SetBattleOffset);
        // Run the menu, allowing user interaction
        menu->Run();
        // After the menu is closed, delete the menu object to free up memory
        delete menu;
        // Return 0 to indicate successful execution
        return (0);
    }
}