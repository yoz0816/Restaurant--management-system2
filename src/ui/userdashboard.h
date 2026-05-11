#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QMainWindow>
#include <QVector>
#include "database.h"
#include "linkedlist.h"
#include "menuitem.h"
#include "validation.h"
#include "menu_linkedlist.h"

class QTableWidget;
class QPushButton;
class QLineEdit;
class QLabel;
class QTextEdit;

class UserDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit UserDashboard(Database *db, const QString &username, QWidget *parent = nullptr);
    ~UserDashboard();

signals:
    void logoutRequested();

private slots:
    void onAddToOrderClicked();
    void onRemoveFromOrderClicked();
    void onGenerateBillClicked();
    void loadMenu();

    void onSearchClicked();   

    void onLogoutClicked();
    void onExitClicked();

private:
    void setupUI();
    void populateMenuTable();
    void populateOrderTable();

    Database *db;
    QString username;

    QTableWidget *menuTable;
    QTableWidget *orderTable;

    QLineEdit *quantityEdit;

    QLineEdit *searchEdit;
    QPushButton *searchButton;

    QPushButton *addToOrderButton;
    QPushButton *removeFromOrderButton;
    QPushButton *generateBillButton;

    QPushButton *logoutButton;
    QPushButton *exitButton;

    QTextEdit *billText;

    MenuLinkedList menuItems;
    LinkedList orderList;    
};

#endif