#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamewindow.h"
#include "mainmenu.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->logoLabel->setPixmap(QPixmap(":/LOGO.png"));
    ui->logoLabel->setScaledContents(true);

    QString styleSheet = R"(
        
        QWidget#MainWindow {
            background-color: #1e1e1e;
        }

        QLabel {
            font-size: 14px;
            font-weight: bold;
            color: #FFFFFF;
            padding-left: 3px;
            margin-top: 8px;
        }

        QLabel#logoLabel {
            margin-bottom: 15px;
            min-height: 50px;
        }

        QLineEdit {
            border: 1px solid #444;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            background-color: #333;
            color: white;
            min-height: 25px;
        }

        QPushButton#loginButton {
            background-color: #E91E63;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            font-weight: bold;
            min-height: 30px;
            margin-top: 15px;
        }

        QPushButton#loginButton:hover {
            background-color: #D81B60;
        }

        QPushButton#registerButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            font-weight: bold;
            min-height: 30px;
            margin-top: 5px;
        }

        QPushButton#registerButton:hover {
            background-color: #45a049;
        }

    )";

    this->setStyleSheet(styleSheet);

    m_networkManager = new QNetworkAccessManager(this);
}
MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_registerButton_clicked() {
    QString user = ui->usernameLineEdit->text();
    QString pass = ui->passwordLineEdit->text();
    if (user.isEmpty() || pass.isEmpty()) return;

    QJsonObject json; json["username"] = user; json["password"] = pass;
    QNetworkRequest req(QUrl("http://localhost:18080/api/register"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->post(req, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onRegisterReply(reply); });
    ui->registerButton->setEnabled(false);
}

void MainWindow::on_loginButton_clicked() {
    QString user = ui->usernameLineEdit->text();
    QString pass = ui->passwordLineEdit->text();
    if (user.isEmpty() || pass.isEmpty()) return;

    QJsonObject json; json["username"] = user; json["password"] = pass;
    QNetworkRequest req(QUrl("http://localhost:18080/api/login"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->post(req, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onLoginReply(reply); });
    ui->loginButton->setEnabled(false);
}

void MainWindow::onLoginReply(QNetworkReply* reply) {
    ui->loginButton->setEnabled(true);
    if (reply->error() == QNetworkReply::NoError) {
        auto obj = QJsonDocument::fromJson(reply->readAll()).object();
        if (obj["success"].toBool()) {
            MainMenu* menu = new MainMenu(ui->usernameLineEdit->text(), obj["userId"].toInt());
            menu->setAttribute(Qt::WA_DeleteOnClose);
            menu->show();
            this->close();
        }
    }
    reply->deleteLater();
}

void MainWindow::onRegisterReply(QNetworkReply* reply) {
    ui->registerButton->setEnabled(true);
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(this, "Success", "Registration successful. You can log in now.");
    }
    reply->deleteLater();
}