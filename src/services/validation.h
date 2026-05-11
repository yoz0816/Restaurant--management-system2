#ifndef VALIDATION_H
#define VALIDATION_H

#include <QString>
#include "menuitem.h"
#include "user.h"

class Validation {
public:
    static bool validateMenuItem(const MenuItem& item);
    static bool validateUsername(const QString& username);
    static bool validatePassword(const QString& password);
    static bool validateNewMenuItem(const QString& name, double price, int stock);
};

#endif