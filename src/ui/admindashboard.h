#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QMainWindow>

#include "database.h"
#include "menuitem.h"
#include "menu_linkedlist.h"
#include "validation.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QComboBox;
class QTableWidget;

class AdminDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminDashboard(Database *db, QWidget *parent = nullptr);
    ~AdminDashboard();

signals:
    void logoutRequested();

private slots:
    void onAddItemClicked();
    void onUpdateItemClicked();
    void onDeleteItemClicked();
    void onSortClicked();
    void onSearchClicked();
    void onViewSalesClicked();
    void onViewUsersClicked();
    void onLogoutClicked();
    void onExitClicked();

private:
    void setupUI();
    void loadMenu();
    void populateTable();
    void showSaleInvoice(int orderId);

    Database *db;

    QLineEdit *nameEdit;
    QLineEdit *priceEdit;
    QLineEdit *stockEdit;

    QPushButton *addButton;
    QPushButton *updateButton;
    QPushButton *deleteButton;
    QPushButton *sortButton;
    QPushButton *searchButton;
    QPushButton *salesButton;
    QPushButton *viewUsersButton;

    QPushButton *logoutButton;
    QPushButton *exitButton;

    QComboBox *sortCombo;
    QComboBox *searchCombo;
    QLineEdit *searchEdit;

    QTableWidget *menuTable;

    MenuLinkedList menuItems;
};

#endif