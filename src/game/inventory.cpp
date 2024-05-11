#include "inventory.hpp"

Inventory::Inventory() = default;

Item *Inventory::get_active_item() {
    int idx = this->active_item_idx;
    if (idx >= 0 && (uint32_t)idx < this->items.size()) {
        return &this->items[idx];
    }

    return nullptr;
}

void Inventory::set_active_item(int item_idx) {
    this->active_item_idx = item_idx;
}

void Inventory::clear_active_item() {
    this->active_item_idx = -1;
}
