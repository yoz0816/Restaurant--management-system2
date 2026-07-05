#ifndef MENUITEM_H
#define MENUITEM_H

#include <QString>

struct MenuItem {
    int id;
    QString name;
    double price;
    int stock;
    int sold;

    MenuItem()
        : id(0), name(""), price(0.0), stock(0), sold(0) {}

    MenuItem(int i, const QString& n, double p, int s, int so)
        : id(i), name(n), price(p), stock(s), sold(so) {}

    MenuItem(int i, const QString& n, double p, int s)
        : id(i), name(n), price(p), stock(s), sold(0) {}

    bool isAvailable(int quantity) const {
        return stock >= quantity;
    }

    void reduceStock(int quantity) {
        if (quantity <= stock) {
            stock -= quantity;
            sold += quantity;
        }
    }
};

#endif