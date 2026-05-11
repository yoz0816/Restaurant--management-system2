#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QVariant>
#include <QList>
#include <QVariantMap>
#include <QString>

class Database {
public:
    Database();
    ~Database();

    bool openConnection();
    void closeConnection();
    bool ensureConnection();

    bool authenticateUser(const QString &username, const QString &password, QString &role);
    bool registerUser(const QString &username, const QString &password, const QString &role);
    int getUserId(const QString &username);

    bool addMenuItem(const QString &name, double price, int stock);
    bool updateMenuItem(int id, const QString &name, double price, int stock, int sold = -1);
    bool deleteMenuItem(int id);

    QList<QVariantMap> getMenuItems(const QString& sortBy = "id", bool ascending = true);
    QList<QVariantMap> getMenuItemsByName(const QString& name);
    QVariantMap getMenuItem(int id);

    int createOrder(int userId, double total);
    bool addOrderItem(int orderId, int menuId, int quantity, double price);

    QList<QVariantMap> getSalesReport();

private:
    QSqlDatabase db;
};

#endif 