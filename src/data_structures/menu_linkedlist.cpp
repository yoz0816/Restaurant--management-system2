#include "menu_linkedlist.h"
#include <QPair>
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
        bool match = false;
        if (itemName.startsWith(query)) {
            match = true;
        } else if (itemName.contains(" " + query)) {
       
            match = true;
        } else if (query.size() >= 3 && itemName.contains(query)) {
          
            match = true;
        }

        if (match) results.append(index);
        temp = temp->next;
        index++;
    }

    return results;
}

QList<int> MenuLinkedList::binarySearchAllByName(const QString& name) const {
    QList<int> results;
    QString query = name.trimmed().toLower();
    if (query.isEmpty()) return results;

    QList<MenuItem> items = toList();
    QList<QPair<QString, int>> indexedNames;
    for (int i = 0; i < items.size(); ++i) {
        indexedNames.append({items[i].name.trimmed().toLower(), i});
    }

    std::sort(indexedNames.begin(), indexedNames.end(), [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
        return a.first < b.first;
    });

    int left = 0;
    int right = indexedNames.size() - 1;
    int found = -1;

    while (left <= right) {
        int mid = (left + right) / 2;
        const QString& midName = indexedNames[mid].first;

        if (midName.startsWith(query)) {
            found = mid;
            break;
        }

        if (midName < query) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (found == -1) {
       
        return results;
    }

    int first = found;
    while (first > 0 && indexedNames[first - 1].first.startsWith(query)) {
        first--;
    }

    int last = found;
    while (last + 1 < indexedNames.size() && indexedNames[last + 1].first.startsWith(query)) {
        last++;
    }

    for (int i = first; i <= last; ++i) {
        results.append(indexedNames[i].second);
    }

 
    for (int i = 0; i < items.size(); ++i) {
        const QString iname = items[i].name.trimmed().toLower();
        if (iname.startsWith(query)) continue;

        bool match = false;

        QStringList parts = iname.split(' ', Qt::SkipEmptyParts);
        for (const QString &p : parts) {
            if (p.startsWith(query)) {
                match = true;
                break;
            }
        }

    
        if (!match && query.size() >= 3 && iname.contains(query)) match = true;

        if (match) results.append(i);
    }

    std::sort(results.begin(), results.end());
    results.erase(std::unique(results.begin(), results.end()), results.end());
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
            const QString currentName = current->item.name.trimmed().toLower();
            const QString nextName = current->next->item.name.trimmed().toLower();

            bool condition = ascending
                ? (current->item.price > current->next->item.price
                   || (current->item.price == current->next->item.price && currentName > nextName))
                : (current->item.price < current->next->item.price
                   || (current->item.price == current->next->item.price && currentName > nextName));

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
            const QString currentName = current->item.name.trimmed().toLower();
            const QString targetName = target->item.name.trimmed().toLower();

            bool condition = descending
                ? (current->item.sold > target->item.sold
                   || (current->item.sold == target->item.sold && currentName < targetName))
                : (current->item.sold < target->item.sold
                   || (current->item.sold == target->item.sold && currentName < targetName));

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