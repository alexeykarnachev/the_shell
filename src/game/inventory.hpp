#pragma once

#include "item.hpp"
#include <vector>

class Inventory {
private:
    int active_item_idx = -1;

public:
    std::vector<Item> items;

    Inventory();

    Item *get_active_item();
    void set_active_item(int item_idx);
    void clear_active_item();
};
