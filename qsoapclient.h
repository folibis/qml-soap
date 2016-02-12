#ifndef QSOAP_H
#define QSOAP_H

#include <QObject>
#include <QVariant>
#include <QDomDocument>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;

class QSoapClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
public:
    explicit QSoapClient(QObject *parent = 0);
    Q_INVOKABLE bool call(const QString &function,const QVariantMap &arguments,const QString &ns = QString::null);

    QUrl url();
    void setUrl(const QUrl &url);

    QString user();
    void setUser(const QString &user);

    QString password();
    void setPassword(const QString &password);

protected:
    QString createMessage(const QString &function, const QVariantMap &arguments, const QString &ns);
    void readMessage(const QByteArray &data);
    void addNode(QDomDocument &doc, QDomElement &parent, const QVariantMap &arguments);
    QVariantMap getNode(QDomNode &root);
    void parseError(QDomNode root);
    QDomNode findNode(QDomNode &parent,const QString &name);

private:
    QUrl p_url;
    QString p_user;
    QString p_password;
    QNetworkAccessManager *p_manager;

signals:
    void urlChanged();
    void userChanged();
    void passwordChanged();
    void messageReceived(QVariantMap data);
    void error(const QString &error);

public slots:
    void finished(QNetworkReply * reply);
    void authenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator);
};

#endif // QSOAP_H
