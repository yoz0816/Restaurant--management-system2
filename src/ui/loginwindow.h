#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "database.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QRadioButton;
class QButtonGroup;

class LoginWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onModeChanged();
    void showLoginAgain();

private:
    void setupUI();

    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;

    QPushButton *loginButton;
    QPushButton *registerButton;

    QLabel *modeLabel;
    QRadioButton *loginRadio;
    QRadioButton *registerRadio;
    QButtonGroup *modeGroup;

    Database db;  
};

#endif