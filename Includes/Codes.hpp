#ifndef CHEATS_HPP
#define CHEATS_HPP

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"

namespace CTRPluginFramework {
    void UpdateIcon(MenuEntry *entry);
    void UpdateNices(MenuEntry *entry);
    void UpdateWishes(MenuEntry *entry);
    void UpdateProfileMessage(MenuEntry *entry);
    void UpdateHistory(MenuEntry *entry);
    void UpdateLinks(MenuEntry *entry);
    void UpdateMiniSurvey(MenuEntry *entry);
    void UpdateGreets(MenuEntry *entry);
    void PlayerSearchSystem(MenuEntry *entry);
    void UpdateGauge(MenuEntry *entry);
    void Gauge(MenuEntry *entry);
    void UnlockEveryOPower(MenuEntry *entry);
    void UpdateUsers(MenuEntry *entry);
    void ClearUsersList(MenuEntry *entry);

    vector<string> GetBattleParty(vector<string> party);
    void PartyPosition(MenuEntry *entry);
    void StatusCondition(MenuEntry *entry);
    void Stats(MenuEntry *entry);
    void UpdateStats(MenuEntry *entry);
    void HealthAndMana(MenuEntry *entry);
    void UpdateHealthAndMana(MenuEntry *entry);
    void HeldItem(MenuEntry *entry);
    void Moves(MenuEntry *entry);
    void ExpMultiplier(MenuEntry *entry);
    void UpdateExpMultiplier(MenuEntry *entry);
    void AccessBag(MenuEntry *entry);
    void AllowMultipleMegas(MenuEntry *entry);
    bool IsValid(u32 pointer, PK6 *pkmn);
    bool ViewInfoCallback(const Screen &screen);
    void TogglePokemonInfo(MenuEntry *entry);
    void ViewPokemonInfo(MenuEntry *entry);
    void NoWildPokemon(MenuEntry *entry);
    void UpdateWildSpawner(MenuEntry *entry);
    void WildSpawner(MenuEntry *entry);
    void AlwaysShiny(MenuEntry *entry);
    void CaptureRate(MenuEntry *entry);
    void CatchTrainerPokemon(MenuEntry *entry);

    void FastWalkRun(MenuEntry *entry);
    void WalkThroughWalls(MenuEntry *entry);
    void StayInAction(MenuEntry *entry);
    void ActionMusic(MenuEntry *entry);
    void ApplyMusic(MenuEntry *entry);
    void Teleportation(MenuEntry *entry);
    void FlyMapInSummary(MenuEntry *entry);
    void UnlockFullFlyMap(MenuEntry *entry);
    void RenameAnyPokemon(MenuEntry *entry);
    void LearnAnyTeachable(MenuEntry *entry);
    void InstantEgg(MenuEntry *entry);
    void InstantEggHatch(MenuEntry *entry);
    void ViewValuesInSummary(MenuEntry *entry);
    void NoOutlines(MenuEntry *entry);
    void FastDialogs(MenuEntry *entry);
}

#endif