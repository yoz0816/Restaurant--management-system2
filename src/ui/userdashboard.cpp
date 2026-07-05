#include "userdashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QApplication>
#include <QDialog>
#include <QDateTime>
#include <QFontDatabase>

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include <QSqlDatabase>

UserDashboard::UserDashboard(Database *db, const QString &username, QWidget *parent)
    : QMainWindow(parent), db(db), username(username) {
    setupUI();
    loadMenu();
}

UserDashboard::~UserDashboard() {}

void UserDashboard::setupUI() {
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    layout->addWidget(new QLabel("Menu"));

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(new QLabel("Search:"));

    searchEdit = new QLineEdit;
    searchLayout->addWidget(searchEdit);

    searchButton = new QPushButton("Search");
    searchLayout->addWidget(searchButton);

    layout->addLayout(searchLayout);

    connect(searchButton, &QPushButton::clicked,
            this, &UserDashboard::onSearchClicked);

    QHBoxLayout *sortLayout = new QHBoxLayout;
    sortLayout->addWidget(new QLabel("Sort by:"));

    sortCombo = new QComboBox;
    sortCombo->addItems({
        "Price: low to high",
        "Price: high to low",
        "Popularity: low to high",
        "Popularity: high to low"
    });
    sortLayout->addWidget(sortCombo);

    sortButton = new QPushButton("Sort");
    sortLayout->addWidget(sortButton);
    layout->addLayout(sortLayout);

    connect(sortButton, &QPushButton::clicked,
            this, &UserDashboard::onSortClicked);

    menuTable = new QTableWidget;
    menuTable->setColumnCount(5);
    menuTable->setHorizontalHeaderLabels({"ID", "Name", "Price", "Stock", "Popularity"});
    menuTable->horizontalHeader()->setStretchLastSection(true);
    menuTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    menuTable->setSelectionMode(QAbstractItemView::MultiSelection);
    menuTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    menuTable->setMinimumHeight(320);

    layout->addWidget(menuTable);

    QHBoxLayout *addLayout = new QHBoxLayout;
    addLayout->addWidget(new QLabel("Quantity:"));

    quantityEdit = new QLineEdit;
    addLayout->addWidget(quantityEdit);

    addToOrderButton = new QPushButton("Add to Order");
    addLayout->addWidget(addToOrderButton);

    layout->addLayout(addLayout);

    layout->addWidget(new QLabel("Current Order"));

    orderTable = new QTableWidget;
    orderTable->setColumnCount(4);
    orderTable->setHorizontalHeaderLabels({"ID", "Name", "Quantity", "Price"});
    orderTable->horizontalHeader()->setStretchLastSection(true);
    orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(orderTable);

    QHBoxLayout *orderLayout = new QHBoxLayout;
    removeFromOrderButton = new QPushButton("Remove Item");
    generateBillButton = new QPushButton("Generate Bill");

    orderLayout->addWidget(removeFromOrderButton);
    orderLayout->addWidget(generateBillButton);
    layout->addLayout(orderLayout);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    logoutButton = new QPushButton("Logout");
    exitButton = new QPushButton("Exit");

    bottomLayout->addWidget(logoutButton);
    bottomLayout->addWidget(exitButton);
    layout->addLayout(bottomLayout);

    connect(addToOrderButton, &QPushButton::clicked,
            this, &UserDashboard::onAddToOrderClicked);

    connect(removeFromOrderButton, &QPushButton::clicked,
            this, &UserDashboard::onRemoveFromOrderClicked);

    connect(generateBillButton, &QPushButton::clicked,
            this, &UserDashboard::onGenerateBillClicked);

    connect(logoutButton, &QPushButton::clicked,
            this, &UserDashboard::onLogoutClicked);

    connect(exitButton, &QPushButton::clicked,
            this, &UserDashboard::onExitClicked);

    setWindowTitle("User Dashboard - " + username);
    resize(800, 600);
}

void UserDashboard::loadMenu() {
    menuItems.clear();
    currentSearchResults.clear();
    searchEdit->clear();

    QList<QVariantMap> items = db->getMenuItems();

    for (const QVariantMap &item : items) {
        menuItems.append(MenuItem(
            item["id"].toInt(),
            item["name"].toString(),
            item["price"].toDouble(),
            item["stock"].toInt(),
            item["sold"].toInt()
        ));
    }

    if (sortCombo->currentIndex() >= 0) {
        switch (sortCombo->currentIndex()) {
            case 0:
                menuItems.bubbleSortByPrice(true);
                break;
            case 1:
                menuItems.bubbleSortByPrice(false);
                break;
            case 2:
                menuItems.selectionSortByPopularity(false);
                break;
            case 3:
                menuItems.selectionSortByPopularity(true);
                break;
        }
    }

    populateMenuTable();
}

void UserDashboard::populateMenuTable() {
    menuTable->clearContents();
    menuTable->setRowCount(menuItems.size());

    for (int i = 0; i < menuItems.size(); i++) {
        const MenuItem& item = menuItems.at(i);

        menuTable->setItem(i, 0, new QTableWidgetItem(QString::number(item.id)));
        menuTable->setItem(i, 1, new QTableWidgetItem(item.name));
        menuTable->setItem(i, 2, new QTableWidgetItem(QString::number(item.price)));
        menuTable->setItem(i, 3, new QTableWidgetItem(QString::number(item.stock)));
        menuTable->setItem(i, 4, new QTableWidgetItem(QString::number(item.sold)));
    }
}

void UserDashboard::populateOrderTable() {
    orderTable->clearContents();
    orderTable->setRowCount(0);

    int row = 0;
    orderList.forEach([&](const OrderItem& item) {
        orderTable->insertRow(row);
        orderTable->setItem(row, 0, new QTableWidgetItem(QString::number(item.itemId)));
        orderTable->setItem(row, 1, new QTableWidgetItem(item.itemName));
        orderTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.quantity)));
        orderTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.price)));
        row++;
    });
}

void UserDashboard::onSortClicked() {
    loadMenu();
}

void UserDashboard::onSearchClicked() {

    QString query = searchEdit->text().trimmed();

    if (query.isEmpty()) {
        loadMenu();
        return;
    }

    currentSearchResults = menuItems.binarySearchAllByName(query);

    if (currentSearchResults.isEmpty()) {
        QMessageBox::information(this, "Not Found", "Item not found.");
        return;
    }

    menuTable->clearContents();
    menuTable->setRowCount(currentSearchResults.size());

    for (int rowIndex = 0; rowIndex < currentSearchResults.size(); ++rowIndex) {
        const MenuItem &item = menuItems.at(currentSearchResults[rowIndex]);
        menuTable->setItem(rowIndex, 0, new QTableWidgetItem(QString::number(item.id)));
        menuTable->setItem(rowIndex, 1, new QTableWidgetItem(item.name));
        menuTable->setItem(rowIndex, 2, new QTableWidgetItem(QString::number(item.price)));
        menuTable->setItem(rowIndex, 3, new QTableWidgetItem(QString::number(item.stock)));
        menuTable->setItem(rowIndex, 4, new QTableWidgetItem(QString::number(item.sold)));
    }
}

void UserDashboard::onAddToOrderClicked() {

    int row = menuTable->currentRow();

    if (row < 0) {
        QMessageBox::warning(this, "Error", "Select an item first.");
        return;
    }

    int actualIndex = row;
    if (!currentSearchResults.isEmpty()) {
        if (row >= currentSearchResults.size()) {
            QMessageBox::warning(this, "Error", "Invalid selection.");
            return;
        }
        actualIndex = currentSearchResults[row];
    } else {
        if (row >= menuItems.size()) {
            QMessageBox::warning(this, "Error", "Invalid selection.");
            return;
        }
    }

    const MenuItem& item = menuItems.at(actualIndex);

    bool ok;
    int qty = quantityEdit->text().toInt(&ok);

    if (!ok || qty <= 0) {
        QMessageBox::warning(this, "Error", "Invalid quantity.");
        return;
    }

   
    if (!item.isAvailable(qty)) {
        QMessageBox::warning(this, "Error", "Not enough stock available.");
        return;
    }

    orderList.insertItem(item.id, item.name, qty, item.price);

    populateOrderTable();
    quantityEdit->clear();
}

void UserDashboard::onRemoveFromOrderClicked() {

    int row = orderTable->currentRow();

    if (row < 0) {
        QMessageBox::warning(this, "Error", "Select an item.");
        return;
    }

    int itemId = orderList.getItemIdByIndex(row);

    if (QMessageBox::question(this, "Confirm", "Remove item?")
        == QMessageBox::Yes) {

        orderList.deleteItem(itemId);
        populateOrderTable();
    }
}

void UserDashboard::onGenerateBillClicked() {

    if (!db->ensureConnection()) {
        QMessageBox::warning(this, "Error", "Database not connected.");
        return;
    }

    if (orderList.isEmpty()) {
        QMessageBox::warning(this, "Error", "Order is empty.");
        return;
    }

    QMap<int, QVariantMap> menuMap;
    QList<QVariantMap> allItems = db->getMenuItems();

    for (const QVariantMap& item : allItems) {
        menuMap[item["id"].toInt()] = item;
    }

    int userId = db->getUserId(username);
    if (userId == -1) {
        QMessageBox::warning(this, "Error", "User not found.");
        return;
    }

    bool stockOk = true;
    orderList.forEach([&](const OrderItem& item) {
        QVariantMap mi = menuMap.value(item.itemId);
        if (mi.isEmpty() || item.quantity > mi["stock"].toInt()) {
            stockOk = false;
        }
    });

    if (!stockOk) {
        QMessageBox::warning(this, "Error", "Not enough stock.");
        return;
    }

    double subtotal = orderList.calculateTotal();
    double tax = subtotal * 0.15;
    double total = subtotal + tax;
    QString orderDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString bill;
    bill += "------------------------------\n";
    bill += "   Restaurant Management System\n";
    bill += "------------------------------\n";
    bill += QString("USER: %1\n").arg(username);
    bill += QString("Date: %1\n\n").arg(orderDate);

    bill += QString("%1  %2  %3  %4\n")
            .arg("Qty", 3)
            .arg("Item", -22)
            .arg("Price", 8)
            .arg("Total", 8);
    bill += "------------------------------------------------\n";

    orderList.forEach([&](const OrderItem& item) {
        bill += QString("%1  %2  %3  %4\n")
                .arg(item.quantity, 3)
                .arg(item.itemName, -22)
                .arg(item.price, 8, 'f', 2)
                .arg(item.quantity * item.price, 8, 'f', 2);
    });

    bill += "------------------------------------------------\n";
    bill += QString("%1 %2\n").arg("Subtotal:", -31).arg(QString::number(subtotal, 'f', 2), 8);
    bill += QString("%1 %2\n").arg("Tax (15%):", -31).arg(QString::number(tax, 'f', 2), 8);
    bill += QString("%1 %2\n\n").arg("Total:", -31).arg(QString::number(total, 'f', 2), 8);
    bill += "Thank you for your order!\n";

    QSqlDatabase database = QSqlDatabase::database();

    if (!database.transaction()) {
        QMessageBox::warning(this, "Error", "Transaction failed.");
        return;
    }

    int orderId = db->createOrder(userId, total, orderDate);
    if (orderId == -1) {
        database.rollback();
        QMessageBox::warning(this, "Error", "Order creation failed.");
        return;
    }

    bool success = true;

    orderList.forEach([&](const OrderItem& item) {

      if (!success) return; 

      if (!db->addOrderItem(orderId, item.itemId, item.quantity, item.price)) {
            success = false;
            return;
    }
     
    QVariantMap mi = menuMap.value(item.itemId);

        int newStock = mi["stock"].toInt() - item.quantity;
        int newSold  = mi["sold"].toInt() + item.quantity;

        if (!db->updateMenuItem(item.itemId,
                               mi["name"].toString(),
                               mi["price"].toDouble(),
                               newStock,
                               newSold)) {
            success = false;
            return;
        }
    });

    if (!success || !database.commit()) {
        database.rollback();
        QMessageBox::warning(this, "Error", "Transaction failed.");
        return;
    }

    QDialog receiptDialog(this);
    receiptDialog.setWindowTitle("Receipt");
    receiptDialog.resize(520, 420);

    QVBoxLayout *receiptLayout = new QVBoxLayout(&receiptDialog);
    QTextEdit *receiptText = new QTextEdit;
    receiptText->setReadOnly(true);
    receiptText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    receiptText->setPlainText(bill);
    receiptLayout->addWidget(receiptText);

    QPushButton *closeReceiptButton = new QPushButton("Close");
    receiptLayout->addWidget(closeReceiptButton);
    connect(closeReceiptButton, &QPushButton::clicked, &receiptDialog, &QDialog::accept);

    receiptDialog.exec();

    orderList.clear();
    populateOrderTable();
    loadMenu();
}

void UserDashboard::onLogoutClicked() {
    if (QMessageBox::question(this, "Logout", "Are you sure?")
        == QMessageBox::Yes) {
        emit logoutRequested();
        close();
    }
}

void UserDashboard::onExitClicked() {
    if (QMessageBox::question(this, "Exit", "Close application?")
        == QMessageBox::Yes) {
        QApplication::quit();
    }
}