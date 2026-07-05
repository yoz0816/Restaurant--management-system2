#include "validation.h"

bool Validation::validateMenuItem(const MenuItem& item) {
    if (item.id <= 0) return false;
    if (item.name.trimmed().isEmpty()) return false;
    if (item.price <= 0) return false;
    if (item.stock < 0) return false;
    if (item.sold < 0) return false;
    return true;
}

bool Validation::validateNewMenuItem(const QString& name, double price, int stock) {
    if (name.trimmed().isEmpty()) return false;
    if (price <= 0) return false;
    if (stock < 0) return false;
    return true;
}

bool Validation::validateUsername(const QString& username) {
    return !username.isEmpty() && username.length() >= 3;
}

bool Validation::validatePassword(const QString& password) {
    return password.length() >= 4;
}