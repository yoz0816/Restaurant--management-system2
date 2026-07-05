#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "order.h"
#include <QList>
#include <functional>

struct OrderNode {
    OrderItem data;
    OrderNode* next;

    OrderNode(const OrderItem& item) : data(item), next(nullptr) {}
};

class LinkedList {
public:
    LinkedList();
    ~LinkedList();
    
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void insertItem(int itemId, const QString& itemName, int quantity, double price);
    bool deleteItem(int itemId);
    bool updateItem(int itemId, int newQuantity);
    int getItemIdByIndex(int index) const;
    OrderItem* searchItem(int itemId);

    QList<OrderItem> toList() const;
    void forEach(std::function<void(const OrderItem&)> func) const;

    double calculateTotal() const;
    void clear();
    bool isEmpty() const;

private:
    OrderNode* head;
    OrderNode* tail;
};

#endif