#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainmenu.h" 
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QPixmap>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->logoLabel->setPixmap(QPixmap(":/LOGO.png"));
    ui->logoLabel->setScaledContents(true);

    QString styleSheet = R"(
        QWidget#MainWindow { background-color: #1e1e1e; }
        QLabel { color: #FFFFFF; font-weight: bold; }
        QLineEdit { padding: 5px; border-radius: 5px; }
        QPushButton { padding: 8px; border-radius: 5px; font-weight: bold; }
        QPushButton#loginButton { background-color: #E91E63; color: white; }
        QPushButton#registerButton { background-color: #4CAF50; color: white; }
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

        MainMenu* menu = new MainMenu(username, this);
        menu->show();
        this->hide();
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