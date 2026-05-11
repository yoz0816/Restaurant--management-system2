#ifndef USER_H
#define USER_H

#include <QString>

struct User {
    int id;
    QString username;
    QString password;
    QString role; 

    User() : id(0), username(""), password(""), role("user") {}

    User(int i, const QString& u, const QString& p, const QString& r)
        : id(i), username(u), password(p), role(r) {}
};

#endif