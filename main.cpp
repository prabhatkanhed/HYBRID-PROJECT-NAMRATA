#include "InventoryManager.hpp"
#include "inventory.h"

#include <cstring>
#include <iostream>
#include <limits>
#include <string>

/* ------------------------------------------------------------------ */
/* Input helpers                                                        */
/* ------------------------------------------------------------------ */

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readPositiveInt(const std::string &prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val > 0) { clearInput(); return val; }
        std::cout << "  [!] Must be a positive integer. Try again.\n";
        clearInput();
    }
}

static int readNonNegativeInt(const std::string &prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= 0) { clearInput(); return val; }
        std::cout << "  [!] Must be 0 or more. Try again.\n";
        clearInput();
    }
}

static float readNonNegativeFloat(const std::string &prompt) {
    float val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= 0.0f) { clearInput(); return val; }
        std::cout << "  [!] Must be 0.00 or more. Try again.\n";
        clearInput();
    }
}

static std::string readNonEmptyString(const std::string &prompt) {
    std::string val;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, val);
        if (!val.empty()) return val;
        std::cout << "  [!] Name must not be empty. Try again.\n";
    }
}

/* Fill an Item from user input.  id is supplied by the caller. */
static Item buildItem(int id) {
    Item it{};
    it.id = id;

    std::string name = readNonEmptyString("  Name        : ");
    /* Safely copy — leave room for null terminator */
    std::strncpy(it.name, name.c_str(), MAX_NAME_LEN - 1);
    it.name[MAX_NAME_LEN - 1] = '\0';

    it.quantity   = readNonNegativeInt  ("  Quantity     : ");
    it.price      = readNonNegativeFloat("  Price ($)    : ");
    it.is_deleted = 0;
    return it;
}

/* ------------------------------------------------------------------ */
/* Menu actions                                                         */
/* ------------------------------------------------------------------ */

static void doAdd(InventoryManager &mgr) {
    std::cout << "\n--- Add Item ---\n";
    int id = readPositiveInt("  ID          : ");
    Item it = buildItem(id);
    if (mgr.addItem(it))
        std::cout << "  [OK] Item added.\n";
}

static void doView(InventoryManager &mgr) {
    std::cout << "\n--- View Item ---\n";
    int id = readPositiveInt("  ID to view  : ");
    mgr.viewItem(id);
}

static void doUpdate(InventoryManager &mgr) {
    std::cout << "\n--- Update Item ---\n";
    int id = readPositiveInt("  ID to update: ");

    /* Show current record first so the user knows what they're editing */
    Item cur{};
    if (!get_item(id, &cur)) {
        std::cout << "  [Error] Item not found or deleted.\n";
        return;
    }
    std::cout << "  Current name: " << cur.name
              << "  qty: " << cur.quantity
              << "  price: $" << cur.price << "\n";
    std::cout << "  Enter new values:\n";

    Item updated = buildItem(id);
    if (mgr.updateItem(id, updated))
        std::cout << "  [OK] Item updated.\n";
}

static void doDelete(InventoryManager &mgr) {
    std::cout << "\n--- Delete Item ---\n";
    int id = readPositiveInt("  ID to delete: ");
    if (mgr.deleteItem(id))
        std::cout << "  [OK] Item soft-deleted.\n";
}

static void doList(InventoryManager &mgr) {
    std::cout << "\n--- List All Items ---\n";
    std::cout << "  Sort by: (1) ID  (2) Name  [1]: ";
    int choice = 1;
    std::cin >> choice;
    clearInput();
    mgr.listAll(choice == 2);
}

/* ------------------------------------------------------------------ */
/* Main                                                                 */
/* ------------------------------------------------------------------ */

int main() {
    InventoryManager mgr;

    std::cout << "====================================\n";
    std::cout << "   Inventory Manager  v1.0\n";
    std::cout << "   Data file: " << DATA_FILE << "\n";
    std::cout << "====================================\n";

    while (true) {
        std::cout << "\n  1  Add item\n"
                     "  2  View item\n"
                     "  3  Update item\n"
                     "  4  Delete item\n"
                     "  5  List all\n"
                     "  6  Exit\n"
                     "  Choice: ";

        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }
        clearInput();

        switch (choice) {
            case 1: doAdd   (mgr); break;
            case 2: doView  (mgr); break;
            case 3: doUpdate(mgr); break;
            case 4: doDelete(mgr); break;
            case 5: doList  (mgr); break;
            case 6:
                std::cout << "\n  Goodbye!\n";
                return 0;
            default:
                std::cout << "  [!] Invalid choice — enter 1-6.\n";
        }
    }
}
