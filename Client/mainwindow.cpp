#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainmenu.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    setupUiElements();
    m_networkManager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupUiElements()
{
    m_ui->logoLabel->setPixmap(QPixmap(m_logoPath));
    m_ui->logoLabel->setScaledContents(true);
}

void MainWindow::on_registerButton_clicked()
{
    executeAuthRequest(m_apiRegisterUrl);
    m_ui->registerButton->setEnabled(false);
}

void MainWindow::on_loginButton_clicked()
{
    executeAuthRequest(m_apiLoginUrl);
    m_ui->loginButton->setEnabled(false);
}

void MainWindow::executeAuthRequest(const QString& url)
{
    QString user = m_ui->usernameLineEdit->text();
    QString pass = m_ui->passwordLineEdit->text();

    if (user.isEmpty() || pass.isEmpty()) return;

    QJsonObject json;
    json[m_jsonUser] = user;
    json[m_jsonPass] = pass;

    QNetworkRequest req((QUrl(url)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, m_contentType);

    QNetworkReply* reply = m_networkManager->post(req, QJsonDocument(json).toJson());

    if (url == m_apiLoginUrl) {
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { onLoginReply(reply); });
    }
    else {
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { onRegisterReply(reply); });
    }
}

void MainWindow::onLoginReply(QNetworkReply* reply)
{
    m_ui->loginButton->setEnabled(true);
    if (reply->error() == QNetworkReply::NoError) {
        auto obj = QJsonDocument::fromJson(reply->readAll()).object();
        if (obj[m_jsonSuccess].toBool()) {
            MainMenu* menu = new MainMenu(m_ui->usernameLineEdit->text(), obj[m_jsonUserId].toInt());
            menu->setAttribute(Qt::WA_DeleteOnClose);
            menu->show();
            this->close();
        }
    }
    reply->deleteLater();
}

void MainWindow::onRegisterReply(QNetworkReply* reply)
{
    m_ui->registerButton->setEnabled(true);
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(this, m_msgRegSuccessTitle, m_msgRegSuccessBody);
    }
    reply->deleteLater();
}