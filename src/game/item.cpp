#include "item.hpp"

Item::Item() = default;

Item::Item(ItemType type, Sprite sprite)
    : type(type)
    , sprite(sprite) {}

bool Item::is_wall() {
    return this->type == ItemType::WALL;
}

bool Item::is_door() {
    return this->type == ItemType::DOOR;
}

bool Item::is_wall_or_door() {
    return this->is_wall() || this->is_door();
}
