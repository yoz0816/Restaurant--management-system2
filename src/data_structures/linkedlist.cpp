#include "linkedlist.h"

LinkedList::LinkedList() : head(nullptr), tail(nullptr) {}

LinkedList::~LinkedList() {
    clear();
}

void LinkedList::insertItem(int itemId, const QString& itemName, int quantity, double price) {
    OrderNode* temp = head;

    while (temp) {
        if (temp->data.itemId == itemId) {
            temp->data.quantity += quantity;
            return;
        }
        temp = temp->next;
    }

    OrderItem item(itemId, itemName, quantity, price);
    OrderNode* newNode = new OrderNode(item);

    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
}

bool LinkedList::deleteItem(int itemId) {
    if (!head) return false;

    if (head->data.itemId == itemId) {
        OrderNode* temp = head;
        head = head->next;

        if (!head) tail = nullptr;

        delete temp;
        return true;
    }

    OrderNode* temp = head;
    while (temp->next && temp->next->data.itemId != itemId) {
        temp = temp->next;
    }

    if (temp->next) {
        OrderNode* toDelete = temp->next;
        temp->next = toDelete->next;

        if (toDelete == tail)
            tail = temp;

        delete toDelete;
        return true;
    }

    return false;
}

bool LinkedList::updateItem(int itemId, int newQuantity) {
    OrderNode* temp = head;

    while (temp) {
        if (temp->data.itemId == itemId) {
            temp->data.quantity = newQuantity;
            return true;
        }
        temp = temp->next;
    }

    return false;
}

OrderItem* LinkedList::searchItem(int itemId) {
    OrderNode* temp = head;

    while (temp) {
        if (temp->data.itemId == itemId)
            return &(temp->data);

        temp = temp->next;
    }

    return nullptr;
}

QList<OrderItem> LinkedList::toList() const {
    QList<OrderItem> list;
    const OrderNode* temp = head;

    while (temp) {
        list.append(temp->data);
        temp = temp->next;
    }

    return list;
}

void LinkedList::forEach(std::function<void(const OrderItem&)> func) const {
    const OrderNode* temp = head;

    while (temp) {
        func(temp->data);
        temp = temp->next;
    }
}

double LinkedList::calculateTotal() const {
    double total = 0;
    const OrderNode* temp = head;

    while (temp) {
        total += temp->data.quantity * temp->data.price;
        temp = temp->next;
    }

    return total;
}
int LinkedList::getItemIdByIndex(int index) const {
    const OrderNode* temp = head;
    int i = 0;

    while (temp) {
        if (i == index)
            return temp->data.itemId;

        temp = temp->next;
        i++;
    }

    return -1;
}

void LinkedList::clear() {
    while (head) {
        OrderNode* temp = head;
        head = head->next;
        delete temp;
    }

    tail = nullptr;
}

bool LinkedList::isEmpty() const {
    return head == nullptr;
}