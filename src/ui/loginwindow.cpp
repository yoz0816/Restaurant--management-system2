#include "loginwindow.h"
#include "admindashboard.h"
#include "userdashboard.h"
#include "validation.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();

    if (!db.ensureConnection()) {
        QMessageBox::critical(this, "Database Error",
                              "Unable to open SQLite database (restaurant.db).");
    }
}

LoginWindow::~LoginWindow() {}

void LoginWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    modeLabel = new QLabel("Select Mode:");
    layout->addWidget(modeLabel);

    loginRadio = new QRadioButton("Login");
    registerRadio = new QRadioButton("Register");
    loginRadio->setChecked(true);

    modeGroup = new QButtonGroup(this);
    modeGroup->addButton(loginRadio);
    modeGroup->addButton(registerRadio);

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(loginRadio);
    modeLayout->addWidget(registerRadio);
    layout->addLayout(modeLayout);

    layout->addWidget(new QLabel("Username:"));
    usernameEdit = new QLineEdit;
    layout->addWidget(usernameEdit);

    layout->addWidget(new QLabel("Password:"));
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordEdit);

    loginButton = new QPushButton("Login");
    registerButton = new QPushButton("Register");
    registerButton->setVisible(false);

    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    connect(loginRadio, &QRadioButton::toggled,
            this, &LoginWindow::onModeChanged);

    connect(loginButton, &QPushButton::clicked,
            this, &LoginWindow::onLoginClicked);

    connect(registerButton, &QPushButton::clicked,
            this, &LoginWindow::onRegisterClicked);

    connect(passwordEdit, &QLineEdit::returnPressed,
            this, &LoginWindow::onLoginClicked);

    setWindowTitle("RMS Login");
    resize(320, 220);
}

void LoginWindow::onModeChanged() {
    bool isLogin = loginRadio->isChecked();
    loginButton->setVisible(isLogin);
    registerButton->setVisible(!isLogin);

    usernameEdit->clear();
    passwordEdit->clear();
}

void LoginWindow::onLoginClicked() {

    if (!db.ensureConnection()) {
        QMessageBox::critical(this, "DB Error", "Database not connected.");
        return;
    }

    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error",
                             "Please enter username and password.");
        return;
    }

    if (!Validation::validateUsername(username) ||
        !Validation::validatePassword(password)) {
        QMessageBox::warning(this, "Error",
                             "Invalid username or password format.");
        return;
    }

    QString role;

    if (db.authenticateUser(username, password, role)) {

        QWidget *dashboard = nullptr;

        if (role.toUpper() == "ADMIN") {
            dashboard = new AdminDashboard(&db);
        } else {
            dashboard = new UserDashboard(&db, username);
        }

        connect(dashboard, &QWidget::destroyed,
                this, &LoginWindow::showLoginAgain);

        dashboard->setAttribute(Qt::WA_DeleteOnClose);
        dashboard->show();

        this->hide();

    } else {
        QMessageBox::warning(this, "Login Failed",
                             "Invalid username or password.");
        passwordEdit->clear();
    }
}
void LoginWindow::onRegisterClicked() {

    if (!db.ensureConnection()) {
        QMessageBox::critical(this, "DB Error", "Database not connected.");
        return;
    }

    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error",
                             "Please fill all fields.");
        return;
    }

    if (!Validation::validateUsername(username) ||
        !Validation::validatePassword(password)) {
        QMessageBox::warning(this, "Error",
                             "Invalid username or password format.");
        return;
    }

    if (db.registerUser(username, password, "CASHIER")) {
        QMessageBox::information(this, "Success",
                                 "User registered successfully.");

        loginRadio->setChecked(true);
        onModeChanged();
    } else {
        QMessageBox::warning(this, "Error",
                             "Username already exists.");
    }
}

void LoginWindow::showLoginAgain() {
    this->show();
    usernameEdit->clear();
    passwordEdit->clear();
}