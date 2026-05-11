#include "menu_linkedlist.h"
#include <stdexcept>
#include <algorithm>

MenuLinkedList::MenuLinkedList() : head(nullptr), tail(nullptr), count(0) {}

MenuLinkedList::~MenuLinkedList() {
    clear();
}

void MenuLinkedList::append(const MenuItem& item) {
    MenuNode* newNode = new MenuNode(item);

    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }

    count++;
}

bool MenuLinkedList::removeById(int id) {
    if (!head) return false;

    if (head->item.id == id) {
        MenuNode* temp = head;
        head = head->next;

        if (!head) tail = nullptr;

        delete temp;
        count--;
        return true;
    }

    MenuNode* curr = head;
    while (curr->next && curr->next->item.id != id) {
        curr = curr->next;
    }

    if (curr->next) {
        MenuNode* toDelete = curr->next;
        curr->next = toDelete->next;

        if (toDelete == tail)
            tail = curr;

        delete toDelete;
        count--;
        return true;
    }

    return false;
}

bool MenuLinkedList::updateItem(int id, const MenuItem& newItem) {
    MenuNode* temp = head;

    while (temp) {
        if (temp->item.id == id) {
            temp->item = newItem;
            return true;
        }
        temp = temp->next;
    }

    return false;
}

int MenuLinkedList::findById(int id) const {
    const MenuNode* temp = head;
    int index = 0;

    while (temp) {
        if (temp->item.id == id)
            return index;

        temp = temp->next;
        index++;
    }

    return -1;
}

QList<int> MenuLinkedList::findAllByName(const QString& name) const {
    QList<int> results;
    QString query = name.trimmed().toLower();
    if (query.isEmpty()) return results;

    const MenuNode* temp = head;
    int index = 0;

    while (temp) {
        QString itemName = temp->item.name.trimmed().toLower();
        if (itemName.contains(query)) {
            results.append(index);
        }
        temp = temp->next;
        index++;
    }

    return results;
}

MenuItem* MenuLinkedList::getById(int id) {
    MenuNode* temp = head;

    while (temp) {
        if (temp->item.id == id)
            return &(temp->item);

        temp = temp->next;
    }

    return nullptr;
}

MenuItem& MenuLinkedList::at(int index) {
    if (index < 0 || index >= count)
        throw std::out_of_range("Index out of range");

    MenuNode* temp = head;
    for (int i = 0; i < index; i++) {
        temp = temp->next;
    }

    return temp->item;
}

const MenuItem& MenuLinkedList::at(int index) const {
    if (index < 0 || index >= count)
        throw std::out_of_range("Index out of range");

    const MenuNode* temp = head;
    for (int i = 0; i < index; i++) {
        temp = temp->next;
    }

    return temp->item;
}

int MenuLinkedList::size() const {
    return count;
}

bool MenuLinkedList::isEmpty() const {
    return head == nullptr;
}

void MenuLinkedList::clear() {
    while (head) {
        MenuNode* temp = head;
        head = head->next;
        delete temp;
    }

    tail = nullptr;
    count = 0;
}

void MenuLinkedList::bubbleSortByPrice(bool ascending) {
    if (!head || !head->next) return;

    bool swapped;
    do {
        swapped = false;
        MenuNode* current = head;

        while (current->next) {
            bool condition = ascending
                ? (current->item.price > current->next->item.price)
                : (current->item.price < current->next->item.price);

            if (condition) {
                std::swap(current->item, current->next->item);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}

void MenuLinkedList::selectionSortByPrice(bool ascending) {
    if (!head || !head->next) return;

    MenuNode* start = head;

    while (start->next) {
        MenuNode* target = start;
        MenuNode* current = start->next;

        while (current) {
            bool condition = ascending
                ? (current->item.price < target->item.price)
                : (current->item.price > target->item.price);

            if (condition)
                target = current;

            current = current->next;
        }

        if (target != start)
            std::swap(start->item, target->item);

        start = start->next;
    }
}

void MenuLinkedList::bubbleSortByPopularity(bool descending) {
    if (!head || !head->next) return;

    bool swapped;
    do {
        swapped = false;
        MenuNode* current = head;

        while (current->next) {
            bool condition = descending
                ? (current->item.sold < current->next->item.sold)
                : (current->item.sold > current->next->item.sold);

            if (condition) {
                std::swap(current->item, current->next->item);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}

void MenuLinkedList::selectionSortByPopularity(bool descending) {
    if (!head || !head->next) return;

    MenuNode* start = head;

    while (start->next) {
        MenuNode* target = start;
        MenuNode* current = start->next;

        while (current) {
            bool condition = descending
                ? (current->item.sold > target->item.sold)
                : (current->item.sold < target->item.sold);

            if (condition)
                target = current;

            current = current->next;
        }

        if (target != start)
            std::swap(start->item, target->item);

        start = start->next;
    }
}

void MenuLinkedList::forEach(std::function<void(const MenuItem&)> func) const {
    const MenuNode* temp = head;

    while (temp) {
        func(temp->item);
        temp = temp->next;
    }
}

QList<MenuItem> MenuLinkedList::toList() const {
    QList<MenuItem> list;
    const MenuNode* temp = head;

    while (temp) {
        list.append(temp->item);
        temp = temp->next;
    }

    return list;
}