#include "database.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QSqlDatabase>

Database::Database() {}

Database::~Database() {
    closeConnection();
}

bool Database::openConnection() {

    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection");
        db.setDatabaseName("restaurant.db");
    }

    if (!db.open()) {
        qDebug() << "DB ERROR:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE,"
               "password TEXT,"
               "role TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS menu ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT,"
               "price REAL,"
               "stock INTEGER,"
               "sold INTEGER DEFAULT 0)");

    query.exec("CREATE TABLE IF NOT EXISTS orders ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "user_id INTEGER,"
               "total REAL,"
               "date DATETIME DEFAULT CURRENT_TIMESTAMP)");

    query.exec("CREATE TABLE IF NOT EXISTS order_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "order_id INTEGER,"
               "menu_id INTEGER,"
               "quantity INTEGER,"
               "price REAL)");

    QSqlQuery check(db);

    if (check.exec("SELECT COUNT(*) FROM users WHERE username='admin'")
        && check.next()
        && check.value(0).toInt() == 0) {

        QSqlQuery insert(db);
        insert.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");

        insert.addBindValue("admin");
        insert.addBindValue("1234");
        insert.addBindValue("ADMIN");

        if (!insert.exec()) {
            qDebug() << "Admin creation failed:" << insert.lastError().text();
        } else {
            qDebug() << "Default admin created.";
        }
    }

    return true;
}

bool Database::ensureConnection() {
    if (db.isOpen()) return true;
    return openConnection();
}

void Database::closeConnection() {
    if (db.isOpen()) db.close();
}

bool Database::authenticateUser(const QString &username, const QString &password, QString &role) {

    if (!ensureConnection()) return false;

    QSqlQuery query(db);
    query.prepare("SELECT role FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec()) return false;

    if (query.next()) {
        role = query.value(0).toString();
        return true;
    }

    return false;
}

bool Database::registerUser(const QString &username, const QString &password, const QString &role) {

    if (!ensureConnection()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(role);

    return query.exec();
}

int Database::getUserId(const QString &username) {

    if (!ensureConnection()) return -1;

    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) return -1;

    if (query.next()) return query.value(0).toInt();

    return -1;
}

bool Database::addMenuItem(const QString &name, double price, int stock) {

    if (!ensureConnection()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO menu (name, price, stock) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(price);
    query.addBindValue(stock);

    return query.exec();
}

bool Database::updateMenuItem(int id, const QString &name, double price, int stock, int sold) {

    if (!ensureConnection()) return false;

    QSqlQuery query(db);

    if (sold < 0) {
        query.prepare("UPDATE menu SET name=?, price=?, stock=? WHERE id=?");
        query.addBindValue(name);
        query.addBindValue(price);
        query.addBindValue(stock);
        query.addBindValue(id);
    } 
    else {
        query.prepare("UPDATE menu SET name=?, price=?, stock=?, sold=? WHERE id=?");
        query.addBindValue(name);
        query.addBindValue(price);
        query.addBindValue(stock);
        query.addBindValue(sold);
        query.addBindValue(id);
    }

    return query.exec();
}

bool Database::deleteMenuItem(int id) {

    if (!ensureConnection()) return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM menu WHERE id=?");
    query.addBindValue(id);

    return query.exec();
}

QList<QVariantMap> Database::getMenuItems(const QString& sortBy, bool ascending) {

    QList<QVariantMap> items;
    if (!ensureConnection()) return items;

    QString order = ascending ? "ASC" : "DESC";
    
    QString column;

    if (sortBy == "name" || sortBy == "price" || sortBy == "stock" || sortBy == "sold")
           column = sortBy;
    else
           column = "id";
  
    QString queryStr = QString(
    "SELECT id, name, price, stock, sold FROM menu ORDER BY %1 %2"
    ).arg(column, order);

    QSqlQuery query(db);
    if (!query.exec(queryStr)) {
        qDebug() << query.lastError().text();
        return items;
     }
   
    while (query.next()) {
        QVariantMap item;
        item["id"] = query.value(0);
        item["name"] = query.value(1);
        item["price"] = query.value(2);
        item["stock"] = query.value(3);
        item["sold"] = query.value(4);
        items.append(item); 
    }

    return items;
}

QList<QVariantMap> Database::getMenuItemsByName(const QString& name) {

    QList<QVariantMap> items;
    if (!ensureConnection()) return items;

    QSqlQuery query(db);
    query.prepare("SELECT id, name, price, stock, sold FROM menu WHERE name LIKE ?");
    query.addBindValue("%" + name + "%");

    if (!query.exec()) return items;

    while (query.next()) {
        QVariantMap item;
        item["id"] = query.value(0);
        item["name"] = query.value(1);
        item["price"] = query.value(2);
        item["stock"] = query.value(3);
        item["sold"] = query.value(4);
        items.append(item);
    }

    return items;
}

QVariantMap Database::getMenuItem(int id) {

    QVariantMap item;
    if (!ensureConnection()) return item;

    QSqlQuery query(db);
    query.prepare("SELECT id, name, price, stock, sold FROM menu WHERE id=?");
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        item["id"] = query.value(0);
        item["name"] = query.value(1);
        item["price"] = query.value(2);
        item["stock"] = query.value(3);
        item["sold"] = query.value(4);
    }

    return item;
}

int Database::createOrder(int userId, double total) {

    if (!ensureConnection()) return -1;

    QSqlQuery query(db);
    query.prepare("INSERT INTO orders (user_id, total) VALUES (?, ?)");
    query.addBindValue(userId);
    query.addBindValue(total);

    if (!query.exec()) return -1;

    return query.lastInsertId().toInt();
}

bool Database::addOrderItem(int orderId, int menuId, int quantity, double price) {

    if (!ensureConnection()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO order_items (order_id, menu_id, quantity, price) VALUES (?, ?, ?, ?)");
    query.addBindValue(orderId);
    query.addBindValue(menuId);
    query.addBindValue(quantity);
    query.addBindValue(price);

    return query.exec();
}
QList<QVariantMap> Database::getSalesReport()
{
    QList<QVariantMap> sales;
    if (!ensureConnection()) return sales;

    QSqlQuery query(db);

    if (!query.exec("SELECT id, total, date FROM orders")) {
        qDebug() << "Sales query error:" << query.lastError().text();
        return sales;
    }

    while (query.next()) {
        QVariantMap s;
        s["sale_id"] = query.value(0);
        s["total_amount"] = query.value(1);
        s["date"] = query.value(2);
        sales.append(s);
    }

    return sales;
}
