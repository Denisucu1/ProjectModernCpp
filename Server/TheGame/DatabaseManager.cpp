#include "DatabaseManager.h"

Storage& getStorage() {
    static Storage storage = initStorage();
    static bool synced = (storage.sync_schema(), true);
    return storage;
}