#ifndef INVENTORY_MANAGER_HPP
#define INVENTORY_MANAGER_HPP

#include "inventory.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class InventoryManager {
public:
    /* ---- CRUD wrappers -------------------------------------------- */

    bool addItem(const Item &item) {
        if (add_item(&item)) return true;
        std::cerr << "  [Error] Could not add item — duplicate ID or write failure.\n";
        return false;
    }

    bool viewItem(int id) {
        Item out{};
        if (get_item(id, &out)) {
            printHeader();
            printRow(out);
            printFooter();
            return true;
        }
        std::cerr << "  [Error] Item not found or has been deleted.\n";
        return false;
    }

    bool updateItem(int id, const Item &updated) {
        if (update_item(id, &updated)) return true;
        std::cerr << "  [Error] Could not update — item not found or deleted.\n";
        return false;
    }

    bool deleteItem(int id) {
        if (delete_item(id)) return true;
        std::cerr << "  [Error] Could not delete — item not found or already deleted.\n";
        return false;
    }

    /* ---- List with STL sort --------------------------------------- */

    void listAll(bool sortByName = false) {
        const int MAX = 1024;
        std::vector<Item> items(MAX);

        int count = list_items(items.data(), MAX);
        items.resize(static_cast<size_t>(count));

        if (items.empty()) {
            std::cout << "  (No active items in inventory.)\n";
            return;
        }

        /* STL sort — by name or by id */
        if (sortByName) {
            std::sort(items.begin(), items.end(),
                      [](const Item &a, const Item &b) {
                          return std::string(a.name) < std::string(b.name);
                      });
        } else {
            std::sort(items.begin(), items.end(),
                      [](const Item &a, const Item &b) {
                          return a.id < b.id;
                      });
        }

        printHeader();
        for (const auto &it : items) printRow(it);
        printFooter();
        std::cout << "  " << count << " item(s) listed.\n";
    }

private:
    /* ---- Pretty-print helpers ------------------------------------- */

    static void printHeader() {
        std::cout << "\n  "
                  << std::left
                  << std::setw(6)  << "ID"
                  << std::setw(22) << "Name"
                  << std::setw(10) << "Qty"
                  << std::setw(10) << "Price"
                  << "\n";
        std::cout << "  " << std::string(48, '-') << "\n";
    }

    static void printRow(const Item &it) {
        std::cout << "  "
                  << std::left
                  << std::setw(6)  << it.id
                  << std::setw(22) << it.name
                  << std::setw(10) << it.quantity
                  << "$" << std::fixed << std::setprecision(2) << it.price
                  << "\n";
    }

    static void printFooter() {
        std::cout << "  " << std::string(48, '-') << "\n";
    }
};

#endif /* INVENTORY_MANAGER_HPP */
