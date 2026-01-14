#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainmenu.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUiElements();
    m_networkManager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiElements()
{
    ui->logoLabel->setPixmap(QPixmap(logoPath));
    ui->logoLabel->setScaledContents(true);
}

void MainWindow::on_registerButton_clicked()
{
    executeAuthRequest(apiRegisterUrl);
    ui->registerButton->setEnabled(false);
}

void MainWindow::on_loginButton_clicked()
{
    executeAuthRequest(apiLoginUrl);
    ui->loginButton->setEnabled(false);
}

void MainWindow::executeAuthRequest(const QString& url)
{
    QString user = ui->usernameLineEdit->text();
    QString pass = ui->passwordLineEdit->text();

    if (user.isEmpty() || pass.isEmpty()) return;

    QJsonObject json;
    json["username"] = user;
    json["password"] = pass;

    QNetworkRequest req((QUrl(url)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->post(req, QJsonDocument(json).toJson());

    if (url == apiLoginUrl) {
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { onLoginReply(reply); });
    }
    else {
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { onRegisterReply(reply); });
    }
}

void MainWindow::onLoginReply(QNetworkReply* reply)
{
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

void MainWindow::onRegisterReply(QNetworkReply* reply)
{
    ui->registerButton->setEnabled(true);
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(this, "Success", "Registration successful. You can log in now.");
    }
    reply->deleteLater();
}