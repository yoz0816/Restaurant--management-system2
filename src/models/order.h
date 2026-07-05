#ifndef ORDER_H
#define ORDER_H

#include <QString>

struct OrderItem {
    int itemId;
    QString itemName;
    int quantity;
    double price;

    OrderItem()
        : itemId(0), itemName(""), quantity(0), price(0.0) {}

    OrderItem(int id, const QString& name, int qty, double pr)
        : itemId(id), itemName(name), quantity(qty), price(pr) {}
};

#endif