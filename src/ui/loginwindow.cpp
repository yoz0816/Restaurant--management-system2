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
#include <QGuiApplication>
#include <QScreen>
#include <QResizeEvent>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    centerWindow();

    if (!db.ensureConnection()) {
        QMessageBox::critical(this, "Database Error",
                              "Unable to open SQLite database (restaurant.db).");
    }
}

LoginWindow::~LoginWindow() {}

void LoginWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *outerLayout = new QVBoxLayout(centralWidget);
    outerLayout->setContentsMargins(20, 20, 20, 20);
    outerLayout->addStretch();

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addStretch();

    QWidget *formWidget = new QWidget;
    formWidget->setMinimumWidth(420);
    formWidget->setMaximumWidth(460);
    formWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QVBoxLayout *layout = new QVBoxLayout(formWidget);
    layout->setSpacing(12);

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
    modeLayout->addStretch();
    layout->addLayout(modeLayout);

    layout->addWidget(new QLabel("Username:"));
    usernameEdit = new QLineEdit;
    usernameEdit->setMinimumWidth(220);
    usernameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(usernameEdit);

    layout->addWidget(new QLabel("Password:"));
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumWidth(220);
    passwordEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(passwordEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    loginButton = new QPushButton("Login");
    registerButton = new QPushButton("Register");
    registerButton->setVisible(false);
    loginButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    registerButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    layout->addLayout(buttonLayout);
    layout->addStretch();

    middleLayout->addWidget(formWidget);
    middleLayout->addStretch();
    outerLayout->addLayout(middleLayout);
    outerLayout->addStretch();

    connect(loginRadio, &QRadioButton::toggled,
            this, &LoginWindow::onModeChanged);

    connect(loginButton, &QPushButton::clicked,
            this, &LoginWindow::onLoginClicked);

    connect(registerButton, &QPushButton::clicked,
            this, &LoginWindow::onRegisterClicked);

    connect(passwordEdit, &QLineEdit::returnPressed,
            this, &LoginWindow::onLoginClicked);

    setWindowTitle("RMS Login");
    setMinimumSize(520, 380);
    resize(520, 380);
}

void LoginWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    centerWindow();
}

void LoginWindow::centerWindow() {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        return;

    QRect screenGeometry = screen->availableGeometry();
    int x = screenGeometry.x() + (screenGeometry.width() - width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - height()) / 2;
    move(x, y);
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

    if (db.registerUser(username, password, "USER")) {
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