#pragma once

#include "../core/sprite.hpp"

enum class ItemType {
    NONE,
    WALL,
    DOOR,
};

class Item {
public:
    ItemType type = ItemType::NONE;
    Sprite sprite;

    Item();
    Item(ItemType type, Sprite sprite);

    bool is_wall();
    bool is_door();
    bool is_wall_or_door();
};
