#include "sdk.h"

// This method is used to determine the group of a specific entity.
auto ssdk::c_base_entity::get_entity_group(c_base_entity * ent) const -> handle_structs::class_groups::groups
{
    // Define arrays of class IDs for each group.
    constexpr int boss[] = {handle_structs::class_ids::Tank, handle_structs::class_ids::Witch};
    constexpr int special[] = {
        handle_structs::class_ids::Boomer, handle_structs::class_ids::Charger, handle_structs::class_ids::Smoker,
        handle_structs::class_ids::Hunter, handle_structs::class_ids::Jockey, handle_structs::class_ids::Spitter
    };
    constexpr int infected[] = {handle_structs::class_ids::Infected, handle_structs::class_ids::CInsectSwarm};
    constexpr int player[] = {handle_structs::class_ids::CTerrorPlayer, handle_structs::class_ids::SurvivorBot};
    // Get the class ID of the entity.
    const int class_id{ent->get_client_class()->classid};
    // Check if the entity's class ID is in the 'infected' group.
    if (std::find(std::begin(infected), std::end(infected), class_id) != std::end(infected))
        return handle_structs::class_groups::groups::INFECTED;
    // Check if the entity's class ID is in the 'special' group.
    if (std::find(std::begin(special), std::end(special), class_id) != std::end(special))
        return handle_structs::class_groups::groups::SPECIAL;
    // Check if the entity's class ID is in the 'player' group.
    if (std::find(std::begin(player), std::end(player), class_id) != std::end(player))
        return handle_structs::class_groups::groups::PLAYER;
    // Check if the entity's class ID is in the 'boss' group.
    if (std::find(std::begin(boss), std::end(boss), class_id) != std::end(boss))
        return handle_structs::class_groups::groups::STRONK;
    // If the entity's class ID does not match any of the above groups, return 'NONE'.
    return handle_structs::class_groups::groups::NONE;
}

// This method checks if an entity is valid based on its group, flags, and other properties.
auto ssdk::c_base_entity::is_entity_valid() -> bool
{
    // Get the team number of the entity.
    const auto team{g_offsets.get_team_num(this)};
    if (team != 2 && team != 3)
        return false;
    // Get the entity group, sequence, flags, and witch_rage state.
    auto group{get_entity_group(this)};
    auto sequence{g_offsets.get_sequence(this)};
    auto flags{this->get_collideable()->get_solid_flags()};
    auto witch_rage{g_offsets.get_witch_state(this)};
    // Define a lambda function to check the validity of the entity based on its group and properties.
    auto realize_validity = [&](void) -> bool
    {
        if (group == handle_structs::class_groups::NONE) return false;
        if (group == handle_structs::class_groups::STRONK)
        {
            if (flags & 4) return false;
            if (sequence > 70) return false;
            if (witch_rage == true) return false;
        }
        else if (group == handle_structs::class_groups::SPECIAL)
        {
            if (flags & 4) return false;
            if (sequence == 8) return true;
        }
        else if (group == handle_structs::class_groups::INFECTED)
        {
            if (flags & 4) return false;
            if (sequence > 305) return false;
        }
    };
    // Return the result of the validity check.
    return realize_validity();
}
