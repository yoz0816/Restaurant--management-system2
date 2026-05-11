#include "admindashboard.h"
#include "validation.h"

#include <QMessageBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QApplication>

AdminDashboard::AdminDashboard(Database *db, QWidget *parent)
    : QMainWindow(parent), db(db) {
    setupUI();
    loadMenu();
}

AdminDashboard::~AdminDashboard() {}

void AdminDashboard::setupUI() {
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    layout->addWidget(new QLabel("Menu Management"));

    QHBoxLayout *inputLayout = new QHBoxLayout;

    inputLayout->addWidget(new QLabel("Name:"));
    nameEdit = new QLineEdit;
    inputLayout->addWidget(nameEdit);

    inputLayout->addWidget(new QLabel("Price:"));
    priceEdit = new QLineEdit;
    inputLayout->addWidget(priceEdit);

    inputLayout->addWidget(new QLabel("Stock:"));
    stockEdit = new QLineEdit;
    inputLayout->addWidget(stockEdit);

    layout->addLayout(inputLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;

    addButton = new QPushButton("Add Item");
    updateButton = new QPushButton("Update Item");
    deleteButton = new QPushButton("Delete Item");

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);

    layout->addLayout(buttonLayout);

    QHBoxLayout *sortSearchLayout = new QHBoxLayout;

    sortCombo = new QComboBox;
    sortCombo->addItems({
        "Sort by Price (Bubble)",
        "Sort by Price (Selection)",
        "Sort by Popularity (Bubble)",
        "Sort by Popularity (Selection)"
    });

    sortButton = new QPushButton("Sort");

    searchCombo = new QComboBox;
    searchCombo->addItems({
        "Search by ID",
        "Search by Name"
    });

    searchEdit = new QLineEdit;
    searchButton = new QPushButton("Search");

    sortSearchLayout->addWidget(sortCombo);
    sortSearchLayout->addWidget(sortButton);
    sortSearchLayout->addWidget(searchCombo);
    sortSearchLayout->addWidget(searchEdit);
    sortSearchLayout->addWidget(searchButton);

    layout->addLayout(sortSearchLayout);

    menuTable = new QTableWidget;
    menuTable->setColumnCount(5);
    menuTable->setHorizontalHeaderLabels({"ID", "Name", "Price", "Stock", "Sold"});
    menuTable->horizontalHeader()->setStretchLastSection(true);
    menuTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    menuTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(menuTable);

    salesButton = new QPushButton("View Sales Report");
    layout->addWidget(salesButton);

    QHBoxLayout *bottomLayout = new QHBoxLayout;

    logoutButton = new QPushButton("Logout");
    exitButton = new QPushButton("Exit");

    bottomLayout->addWidget(logoutButton);
    bottomLayout->addWidget(exitButton);

    layout->addLayout(bottomLayout);

    connect(addButton, &QPushButton::clicked, this, &AdminDashboard::onAddItemClicked);
    connect(updateButton, &QPushButton::clicked, this, &AdminDashboard::onUpdateItemClicked);
    connect(deleteButton, &QPushButton::clicked, this, &AdminDashboard::onDeleteItemClicked);
    connect(sortButton, &QPushButton::clicked, this, &AdminDashboard::onSortClicked);
    connect(searchButton, &QPushButton::clicked, this, &AdminDashboard::onSearchClicked);
    connect(salesButton, &QPushButton::clicked, this, &AdminDashboard::onViewSalesClicked);

    connect(logoutButton, &QPushButton::clicked, this, &AdminDashboard::onLogoutClicked);
    connect(exitButton, &QPushButton::clicked, this, &AdminDashboard::onExitClicked);

    setWindowTitle("Admin Dashboard");
    resize(800, 600);
}

void AdminDashboard::loadMenu() {
    menuItems.clear();

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

    populateTable();
}

void AdminDashboard::populateTable() {
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

void AdminDashboard::onAddItemClicked() {

    if (!db || !db->ensureConnection()) {
        QMessageBox::critical(this, "DB Error", "Database not connected.");
        return;
    }

    QString name = nameEdit->text().trimmed();

    bool ok1, ok2;
    double price = priceEdit->text().toDouble(&ok1);
    int stock = stockEdit->text().toInt(&ok2);

    if (!ok1 || !ok2 || !Validation::validateNewMenuItem(name, price, stock)) {
        QMessageBox::warning(this, "Error", "Invalid input.");
        return;
    }

    if (db->addMenuItem(name, price, stock)) {
        loadMenu();
        QMessageBox::information(this, "Success", "Item added.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to add item.");
    }
}

void AdminDashboard::onUpdateItemClicked() {

    if (!db || !db->ensureConnection()) return;

    int row = menuTable->currentRow();
    if (row < 0 || row >= menuItems.size()) {
        QMessageBox::warning(this, "Error", "Select a valid row.");
        return;
    }

    const MenuItem& item = menuItems.at(row);

    QString name = nameEdit->text().trimmed();

    bool ok1, ok2;
    double price = priceEdit->text().toDouble(&ok1);
    int stock = stockEdit->text().toInt(&ok2);

    if (!ok1 || !ok2 || name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Invalid input.");
        return;
    }

    if (db->updateMenuItem(item.id, name, price, stock)) {
        loadMenu();
        QMessageBox::information(this, "Success", "Item updated.");
    }
}

void AdminDashboard::onDeleteItemClicked() {

    if (!db || !db->ensureConnection()) return;

    int row = menuTable->currentRow();
    if (row < 0 || row >= menuItems.size()) {
        QMessageBox::warning(this, "Error", "Select a valid row.");
        return;
    }

    const MenuItem& item = menuItems.at(row);

    if (QMessageBox::question(this, "Confirm", "Delete item?")
        == QMessageBox::Yes) {

        if (db->deleteMenuItem(item.id)) {
            loadMenu();
            QMessageBox::information(this, "Success", "Item deleted.");
        }
    }
}

void AdminDashboard::onSortClicked() {
    switch (sortCombo->currentIndex()) {
        case 0: menuItems.bubbleSortByPrice(); break;
        case 1: menuItems.selectionSortByPrice(); break;
        case 2: menuItems.bubbleSortByPopularity(); break;
        case 3: menuItems.selectionSortByPopularity(); break;
    }

    populateTable();
    menuTable->clearSelection();
}

void AdminDashboard::onSearchClicked() {

    QString query = searchEdit->text().trimmed();

    if (query.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter search value.");
        return;
    }

    menuTable->clearSelection();

    if (searchCombo->currentIndex() == 0) {

        bool ok;
        int id = query.toInt(&ok);

        if (!ok) {
            QMessageBox::warning(this, "Error", "Invalid ID.");
            return;
        }

        int row = menuItems.findById(id);

        if (row >= 0) {
            menuTable->selectRow(row);
            menuTable->scrollToItem(menuTable->item(row, 0));
        } else {
            QMessageBox::information(this, "Not Found", "Item not found.");
        }
    }
    else {
        QList<int> results = menuItems.findAllByName(query);

        if (!results.isEmpty()) {
            for (int r : results)
                menuTable->selectRow(r);

            menuTable->scrollToItem(menuTable->item(results.first(), 0));
        } else {
            QMessageBox::information(this, "Not Found", "Item not found.");
        }
    }
}

void AdminDashboard::onViewSalesClicked() {
    QList<QVariantMap> sales = db->getSalesReport();

    if (sales.isEmpty()) {
        QMessageBox::information(this, "Sales", "No sales found.");
        return;
    }

    QString text = "Sales Report:\n\n";

    for (const auto &s : sales) {
        text += "ID: " + s["sale_id"].toString()
             + " | Total: " + s["total_amount"].toString()
             + " | Date: " + s["date"].toString()
             + "\n";
    }

    QMessageBox::information(this, "Sales Report", text);
}

void AdminDashboard::onLogoutClicked() {
    if (QMessageBox::question(this, "Logout", "Are you sure?")
        == QMessageBox::Yes) {
        emit logoutRequested();
        close();
    }
}

void AdminDashboard::onExitClicked() {
    if (QMessageBox::question(this, "Exit", "Close application?")
        == QMessageBox::Yes) {
        QApplication::quit();
    }
}