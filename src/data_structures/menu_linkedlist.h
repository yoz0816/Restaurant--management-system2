#ifndef MENU_LINKEDLIST_H
#define MENU_LINKEDLIST_H

#include "menuitem.h"
#include <QList>
#include <functional>

struct MenuNode {
    MenuItem item;
    MenuNode* next;

    MenuNode(const MenuItem& menuItem) : item(menuItem), next(nullptr) {}
};

class MenuLinkedList {
public:
    MenuLinkedList();
    ~MenuLinkedList();

    MenuLinkedList(const MenuLinkedList&) = delete;
    MenuLinkedList& operator=(const MenuLinkedList&) = delete;

    void append(const MenuItem& item);
    bool removeById(int id);
    bool updateItem(int id, const MenuItem& newItem);

    int findById(int id) const;
    QList<int> findAllByName(const QString& name) const;

    MenuItem* getById(int id);

    MenuItem& at(int index);
    const MenuItem& at(int index) const;

    int size() const;
    bool isEmpty() const;
    void clear();

    void bubbleSortByPrice(bool ascending = true);
    void selectionSortByPrice(bool ascending = true);
    void bubbleSortByPopularity(bool descending = true);
    void selectionSortByPopularity(bool descending = true);

    void forEach(std::function<void(const MenuItem&)> func) const;
    QList<MenuItem> toList() const;

private:
    MenuNode* head;
    MenuNode* tail;
    int count;
};

#endif