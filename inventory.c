#include "inventory.h"

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                     */
/* ------------------------------------------------------------------ */

/* Opens the data file.  mode follows fopen conventions. */
static FILE *open_file(const char *mode)
{
    return fopen(DATA_FILE, mode);
}

/*
 * Scans the file for a record whose id matches.
 * If found, *offset receives the byte offset of that record and
 * *found_item receives a copy of the record; returns 1.
 * Returns 0 if not found.
 */
static int find_record(FILE *fp, int id, long *offset, Item *found_item)
{
    Item tmp;
    long pos = 0;

    rewind(fp);
    while (fread(&tmp, sizeof(Item), 1, fp) == 1) {
        if (tmp.id == id) {
            if (offset)     *offset     = pos;
            if (found_item) *found_item = tmp;
            return 1;
        }
        pos += (long)sizeof(Item);
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Public C API                                                         */
/* ------------------------------------------------------------------ */

int add_item(const Item *item)
{
    FILE *fp;
    Item  tmp;

    if (!item || item->id <= 0)
        return 0;

    /* Reject duplicate IDs (including soft-deleted ones). */
    fp = open_file("rb");
    if (fp) {
        if (find_record(fp, item->id, NULL, &tmp)) {
            fclose(fp);
            return 0;   /* duplicate */
        }
        fclose(fp);
    }

    /* Append the new record. */
    fp = open_file("ab");
    if (!fp) return 0;

    if (fwrite(item, sizeof(Item), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

int get_item(int id, Item *out)
{
    FILE *fp;
    Item  tmp;

    if (!out || id <= 0) return 0;

    fp = open_file("rb");
    if (!fp) return 0;

    if (!find_record(fp, id, NULL, &tmp) || tmp.is_deleted) {
        fclose(fp);
        return 0;
    }
    *out = tmp;
    fclose(fp);
    return 1;
}

int update_item(int id, const Item *updated)
{
    FILE *fp;
    Item  tmp;
    long  offset;

    if (!updated || id <= 0) return 0;

    fp = open_file("r+b");
    if (!fp) return 0;

    if (!find_record(fp, id, &offset, &tmp) || tmp.is_deleted) {
        fclose(fp);
        return 0;
    }

    /* Preserve the original ID and deleted flag. */
    Item to_write   = *updated;
    to_write.id         = id;
    to_write.is_deleted = 0;

    fseek(fp, offset, SEEK_SET);
    int ok = (fwrite(&to_write, sizeof(Item), 1, fp) == 1);
    fclose(fp);
    return ok ? 1 : 0;
}

int delete_item(int id)
{
    FILE *fp;
    Item  tmp;
    long  offset;

    if (id <= 0) return 0;

    fp = open_file("r+b");
    if (!fp) return 0;

    if (!find_record(fp, id, &offset, &tmp) || tmp.is_deleted) {
        fclose(fp);
        return 0;
    }

    tmp.is_deleted = 1;
    fseek(fp, offset, SEEK_SET);
    int ok = (fwrite(&tmp, sizeof(Item), 1, fp) == 1);
    fclose(fp);
    return ok ? 1 : 0;
}

int list_items(Item *buffer, int max_items)
{
    FILE *fp;
    Item  tmp;
    int   count = 0;

    if (!buffer || max_items <= 0) return 0;

    fp = open_file("rb");
    if (!fp) return 0;

    while (fread(&tmp, sizeof(Item), 1, fp) == 1 && count < max_items) {
        if (!tmp.is_deleted)
            buffer[count++] = tmp;
    }
    fclose(fp);
    return count;
}
