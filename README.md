# # Inventory Manager

A console-based inventory CRUD application that combines:

- **C** — binary file I/O (`fread` / `fwrite` / `fseek`) for persistent storage
- **C++** — `InventoryManager` class, `std::vector`, `std::sort`, and an interactive menu

Data survives process restarts because every change is written directly to `inventory.dat`.

---

## File Structure

```
inventory_manager/
├── include/
│   └── inventory.h          # Item struct + C function prototypes
├── src/
│   ├── inventory.c          # C backend (binary file storage)
│   ├── InventoryManager.hpp # C++ class (STL vector/sort, pretty-print)
│   └── main.cpp             # Menu, input validation, entry point
├── Makefile
└── README.md
```

---

## Build & Run

**Requirements:** `gcc`, `g++`, `make` (any modern version).

```bash
# Clone / unzip, then:
cd inventory_manager

make          # compiles everything into ./inventory
make run      # build + launch immediately
make clean    # remove build artefacts and inventory.dat
```

The `Makefile` compiles `*.c` files with `gcc -std=c11` and `*.cpp` files with
`g++ -std=c++17`, then links them with `g++` to include the C++ runtime.

---

## Menu

```
  1  Add item
  2  View item
  3  Update item
  4  Delete item
  5  List all
  6  Exit
```

---

## Test Cases

- **Persistence across restarts**
  Added items 1 (Widget, qty 10, $2.99), 2 (Gadget, qty 5, $14.99), and
  3 (Doohickey, qty 20, $0.50), chose Exit, re-ran the program, and selected
  *List all* — all three items appeared correctly.

- **Duplicate ID rejection**
  Attempted to add a second item with ID 1 while ID 1 already existed.
  The app printed an error and returned to the menu without corrupting the file.

- **Update persists after restart**
  Updated item 2 (Gadget) to qty 99 and price $9.99, exited, re-launched,
  and viewed item 2 — the new values were shown.

- **Soft-delete hides item**
  Deleted item 3 (Doohickey), then chose *List all* — only items 1 and 2
  appeared. Chose *View item* with ID 3 — received "not found" error.
  The binary record remains in the file with `is_deleted = 1`.

- **Input validation**
  Entered `-5` for ID → re-prompted. Entered empty string for name →
  re-prompted. Entered `-1.0` for price → re-prompted. Application never
  crashed; all invalid inputs were rejected gracefully.

---

## Notes

- The data file `inventory.dat` is created automatically on the first `add`.
- Records are fixed-size (`sizeof(Item)` bytes), enabling O(1) random access
  via `fseek` for updates and deletes.
- `list_items` loads up to 1 024 active records into a `std::vector` which is
  then sorted by ID (default) or name before display.
