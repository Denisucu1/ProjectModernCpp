#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_networkManager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_registerButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Eroare inregistrare", "Numele de utilizator nu poate fi gol!");
        return;
    }
    QJsonObject jsonRequest;
    jsonRequest["username"] = username;
    QJsonDocument jsonDoc(jsonRequest);
    QByteArray jsonData = jsonDoc.toJson();
    QUrl registerUrl("http://127.0.0.1:8080/register");
    QNetworkRequest request(registerUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = m_networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::onRegisterReply);
    ui->registerButton->setEnabled(false);
}


void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Eroare Login", "Numele de utilizator nu poate fi gol!");
        return;
    }
    QJsonObject jsonRequest;
    jsonRequest["username"] = username;
    QJsonDocument jsonDoc(jsonRequest);
    QByteArray jsonData = jsonDoc.toJson();
    QUrl loginUrl("http://localhost:18080/api/login");
    QNetworkRequest request(loginUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = m_networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::onLoginReply);
    ui->loginButton->setEnabled(false);
}

void MainWindow::onLoginReply(QNetworkReply *reply)
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
    if (jsonObj["status"].toString() == "success") {
        QMessageBox::information(this, "Login OK", "Te-ai logat cu succes!");
    }
    else {
        QString errorMsg = jsonObj["message"].toString();
        QMessageBox::warning(this, "Eroare Login", "Login esuat: " + errorMsg);
    }
    reply->deleteLater();
}

void MainWindow::onRegisterReply(QNetworkReply *reply)
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
    if (jsonObj["status"].toString() == "success") {
        QMessageBox::information(this, "Inregistrare OK", "Te-ai inregistrat cu succes! Acum te poti loga.");
    } else {
        QString errorMsg = jsonObj["message"].toString();
        QMessageBox::warning(this, "Eroare Inregistrare", "Inregistrare esuata: " + errorMsg);
    }

    reply->deleteLater();
}

