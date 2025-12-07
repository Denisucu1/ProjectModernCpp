#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamewindow.h"
#include "mainmenu.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QVariant>
#include <QPixmap>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_registerButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Eroare Inregistrare", "Username si Parola nu pot fi goale!");
        return;
    }

    QJsonObject jsonRequest;
    jsonRequest["username"] = username;
    jsonRequest["password"] = password;
    QJsonDocument jsonDoc(jsonRequest);
    QByteArray jsonData = jsonDoc.toJson();

    QUrl registerUrl("http://localhost:18080/api/register");
    QNetworkRequest request(registerUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onRegisterReply(reply);
        });
    ui->registerButton->setEnabled(false);
}

void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Eroare Login", "Username si Parola nu pot fi goale!");
        return;
    }

    QJsonObject jsonRequest;
    jsonRequest["username"] = username;
    jsonRequest["password"] = password;
    QJsonDocument jsonDoc(jsonRequest);
    QByteArray jsonData = jsonDoc.toJson();

    QUrl loginUrl("http://localhost:18080/api/login");
    QNetworkRequest request(loginUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onLoginReply(reply);
        });
    ui->loginButton->setEnabled(false);
}

void MainWindow::onLoginReply(QNetworkReply* reply)
{
    ui->loginButton->setEnabled(true);

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Eroare Retea", "Eroare de conexiune: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj["success"].toBool()) {
        QString username = ui->usernameLineEdit->text();

        int userId = jsonObj["userId"].toInt();

        MainMenu* menu = new MainMenu(username, userId, nullptr);

        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->show();
        this->close();
    }
    else {
        QString errorMsg = jsonObj["message"].toString();
        QMessageBox::warning(this, "Eroare Login", "Login esuat: " + errorMsg);
    }

    reply->deleteLater();
}

void MainWindow::onRegisterReply(QNetworkReply* reply)
{
    ui->registerButton->setEnabled(true);
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Eroare Retea", "Eroare de conexiune: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj["success"].toBool()) {
        QMessageBox::information(this, "Inregistrare OK", "Te-ai inregistrat cu succes! Acum te poti loga.");
    }
    else {
        QString errorMsg = jsonObj["message"].toString();
        QMessageBox::warning(this, "Eroare Inregistrare", "Inregistrare esuata: " + errorMsg);
    }

    reply->deleteLater();
}