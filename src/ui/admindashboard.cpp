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
#include <QDialog>
#include <QTextEdit>

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
        "Price: low to high",
        "Price: high to low",
        "Popularity: low to high",
        "Popularity: high to low"
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
    viewUsersButton = new QPushButton("View Users");
    layout->addWidget(salesButton);
    layout->addWidget(viewUsersButton);

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
    connect(viewUsersButton, &QPushButton::clicked, this, &AdminDashboard::onViewUsersClicked);

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

    populateTable();
    menuTable->clearSelection();
}

void AdminDashboard::onViewUsersClicked() {
    QList<QVariantMap> users = db->getUsers();

    QDialog dialog(this);
    dialog.setWindowTitle("User List");
    dialog.resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTableWidget *userTable = new QTableWidget;
    userTable->setColumnCount(3);
    userTable->setHorizontalHeaderLabels({"ID", "Username", "Role"});
    userTable->horizontalHeader()->setStretchLastSection(true);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    userTable->setRowCount(users.size());

    for (int i = 0; i < users.size(); ++i) {
        const QVariantMap &user = users.at(i);
        userTable->setItem(i, 0, new QTableWidgetItem(QString::number(user["id"].toInt())));
        userTable->setItem(i, 1, new QTableWidgetItem(user["username"].toString()));
        userTable->setItem(i, 2, new QTableWidgetItem(user["role"].toString()));
    }

    layout->addWidget(userTable);

    QPushButton *closeButton = new QPushButton("Close");
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.exec();
}

void AdminDashboard::onSearchClicked() {

    QString query = searchEdit->text().trimmed();

    if (query.isEmpty()) {
        loadMenu();
        return;
    }

    if (searchCombo->currentIndex() == 0) {

        bool ok;
        int id = query.toInt(&ok);

        if (!ok) {
            QMessageBox::warning(this, "Error", "Invalid ID.");
            return;
        }

        int row = menuItems.findById(id);

        if (row >= 0) {
            menuTable->clearContents();
            menuTable->setRowCount(1);
            const MenuItem &item = menuItems.at(row);
            menuTable->setItem(0, 0, new QTableWidgetItem(QString::number(item.id)));
            menuTable->setItem(0, 1, new QTableWidgetItem(item.name));
            menuTable->setItem(0, 2, new QTableWidgetItem(QString::number(item.price)));
            menuTable->setItem(0, 3, new QTableWidgetItem(QString::number(item.stock)));
            menuTable->setItem(0, 4, new QTableWidgetItem(QString::number(item.sold)));
        } else {
            QMessageBox::information(this, "Not Found", "Item not found.");
        }
    }
    else {
        QList<int> results = menuItems.findAllByName(query);

        if (!results.isEmpty()) {
            menuTable->clearContents();
            menuTable->setRowCount(results.size());
            for (int rowIndex = 0; rowIndex < results.size(); ++rowIndex) {
                const MenuItem &item = menuItems.at(results[rowIndex]);
                menuTable->setItem(rowIndex, 0, new QTableWidgetItem(QString::number(item.id)));
                menuTable->setItem(rowIndex, 1, new QTableWidgetItem(item.name));
                menuTable->setItem(rowIndex, 2, new QTableWidgetItem(QString::number(item.price)));
                menuTable->setItem(rowIndex, 3, new QTableWidgetItem(QString::number(item.stock)));
                menuTable->setItem(rowIndex, 4, new QTableWidgetItem(QString::number(item.sold)));
            }
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

    QDialog reportDialog(this);
    reportDialog.setWindowTitle("Sales Report");
    reportDialog.resize(640, 360);

    QTableWidget *salesTable = new QTableWidget(&reportDialog);
    salesTable->setColumnCount(3);
    salesTable->setHorizontalHeaderLabels({"Order ID", "Date", "Total"});
    salesTable->setRowCount(sales.count());
    salesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    salesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    salesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (int i = 0; i < sales.count(); ++i) {
        const QVariantMap &sale = sales.at(i);
        salesTable->setItem(i, 0, new QTableWidgetItem(sale["sale_id"].toString()));
        salesTable->setItem(i, 1, new QTableWidgetItem(sale["date"].toString()));
        salesTable->setItem(i, 2, new QTableWidgetItem(QString::number(sale["total_amount"].toDouble(), 'f', 2)));
    }

    QPushButton *invoiceButton = new QPushButton("View Invoice", &reportDialog);
    QPushButton *closeButton = new QPushButton("Close", &reportDialog);

    connect(invoiceButton, &QPushButton::clicked, this, [this, salesTable, &reportDialog]() {
        const QList<QTableWidgetItem*> selected = salesTable->selectedItems();
        if (selected.isEmpty()) {
            QMessageBox::warning(&reportDialog, "Select Order", "Please select an order to view the invoice.");
            return;
        }
        int orderId = salesTable->item(selected.first()->row(), 0)->text().toInt();
        showSaleInvoice(orderId);
    });

    connect(closeButton, &QPushButton::clicked, &reportDialog, &QDialog::accept);

    QVBoxLayout *dialogLayout = new QVBoxLayout(&reportDialog);
    dialogLayout->addWidget(salesTable);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(invoiceButton);
    buttonLayout->addWidget(closeButton);
    dialogLayout->addLayout(buttonLayout);

    reportDialog.exec();
}

void AdminDashboard::showSaleInvoice(int orderId) {
    QList<QVariantMap> items = db->getOrderItems(orderId);
    if (items.isEmpty()) {
        QMessageBox::information(this, "Invoice", "No invoice details found for this order.");
        return;
    }

    QVariantMap orderInfo = db->getOrderSummary(orderId);
    if (orderInfo.isEmpty()) {
        QMessageBox::information(this, "Invoice", "Unable to load order summary.");
        return;
    }

    QString invoice = QString("Receipt\n"
                              "==============================================\n"
                              "Order ID: %1\n"
                              "Date: %2\n"
                              "Customer: %3\n"
                              "==============================================\n"
                              "Item                         Qty   Price   Total\n"
                              "----------------------------------------------\n")
                      .arg(orderInfo["sale_id"].toString())
                      .arg(orderInfo["date"].toString())
                      .arg(orderInfo["username"].toString().isEmpty() ? QString("Guest") : orderInfo["username"].toString());

    double subtotal = 0.0;
    for (const auto &item : items) {
        double qty = item["quantity"].toDouble();
        double price = item["price"].toDouble();
        double itemTotal = qty * price;
        subtotal += itemTotal;

        invoice += QString("%1 %2 %3 %4\n")
                     .arg(item["name"].toString().leftJustified(25, ' '))
                     .arg(QString::number(qty).rightJustified(3, ' '))
                     .arg(QString::number(price, 'f', 2).rightJustified(8, ' '))
                     .arg(QString::number(itemTotal, 'f', 2).rightJustified(10, ' '));
    }

    double tax = subtotal * 0.15;
    double grandTotal = orderInfo["total_amount"].toDouble();

    invoice += "----------------------------------------------\n";
    invoice += QString("Subtotal: %1\n").arg(QString::number(subtotal, 'f', 2), 0, QChar(' '));
    invoice += QString("Tax (15%): %1\n").arg(QString::number(tax, 'f', 2), 0, QChar(' '));
    invoice += QString("Total: %1\n").arg(QString::number(grandTotal, 'f', 2), 0, QChar(' '));
    invoice += "==============================================\n";
    invoice += "Payment: Cash / Card\n";
    invoice += "Thank you for your business!\n";

    QDialog invoiceDialog(this);
    invoiceDialog.setWindowTitle("Order Invoice");
    invoiceDialog.resize(520, 420);

    QTextEdit *invoiceText = new QTextEdit(&invoiceDialog);
    invoiceText->setReadOnly(true);
    invoiceText->setFontFamily("Courier");
    invoiceText->setPlainText(invoice);

    QPushButton *closeButton = new QPushButton("Close", &invoiceDialog);
    connect(closeButton, &QPushButton::clicked, &invoiceDialog, &QDialog::accept);

    QVBoxLayout *layout = new QVBoxLayout(&invoiceDialog);
    layout->addWidget(invoiceText);
    layout->addWidget(closeButton, 0, Qt::AlignRight);

    invoiceDialog.exec();
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