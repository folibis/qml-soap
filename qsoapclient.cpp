#include "qsoapclient.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QAuthenticator>


QSoapClient::QSoapClient(QObject *parent) : QObject(parent)
{
    p_manager = new QNetworkAccessManager(this);
    connect(p_manager,&QNetworkAccessManager::finished,this,&QSoapClient::finished);
    connect(p_manager,&QNetworkAccessManager::authenticationRequired,this,&QSoapClient::authenticationRequired);
}

bool QSoapClient::call(const QString &function, const QVariantMap &arguments,const QString &ns)
{
    QString message = createMessage(function,arguments,ns);
    QNetworkRequest request;
    request.setUrl(p_url);
    request.setRawHeader("Content-Type","text/xml; charset=utf-8");
    p_manager->post(request,message.toUtf8());
    return true;
}

QUrl QSoapClient::url()
{
    return p_url;
}

void QSoapClient::setUrl(const QUrl &url)
{
    if(p_url != url) {
        p_url = url;
        emit urlChanged();
    }
}

QString QSoapClient::user()
{
    return p_user;
}

void QSoapClient::setUser(const QString &user)
{
    if(p_user != user) {
        p_user = user;
        emit userChanged();
    }
}

QString QSoapClient::password()
{
    return p_password;
}

void QSoapClient::setPassword(const QString &password)
{
    if(p_password != password) {
        p_password = password;
        emit passwordChanged();
    }
}

QString QSoapClient::createMessage(const QString &function, const QVariantMap &arguments, const QString &ns)
{
    QDomDocument doc("");
    QDomProcessingInstruction header = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
    doc.appendChild(header);
    QDomElement rootNS = doc.createElementNS("http://www.w3.org/2003/05/soap-envelope", "soap:Envelope");
    rootNS.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
    rootNS.setAttribute("xmlns:xsd","http://www.w3.org/2001/XMLSchema");

    QDomElement body = doc.createElement("soap:Body");

    QDomElement node = doc.createElement(function);
    if(!ns.isNull())
        node.setAttribute("xmlns",ns);
    body.appendChild(node);

    rootNS.appendChild(body);
    doc.appendChild(rootNS);

    addNode(doc,node,arguments);

    return doc.toString();
}

void QSoapClient::readMessage(const QByteArray &data)
{
    QDomDocument doc("");
    QString errorString;
    int errorLine;
    int errorCol;
    if(!doc.setContent(data,true,&errorString,&errorLine,&errorCol)) {
        emit error(QString("XMP parse error: %1 at line %2 column %3")
                   .arg(errorString)
                   .arg(errorLine)
                   .arg(errorCol));
        return;
    }
    QDomElement root = doc.documentElement();
    if(root.localName() == "Envelope") {
        for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling()) {
            if(node.localName() == "Body") {
                if(node.firstChild().localName() == "Fault")
                    parseError(node.firstChild());
                else
                    emit messageReceived(getNode(node));
            }
        }
    }
}

void QSoapClient::addNode(QDomDocument &doc, QDomElement &parent, const QVariantMap &arguments)
{
    QMapIterator<QString, QVariant> i(arguments);
    while (i.hasNext()) {
        i.next();
        QDomElement node = doc.createElement(i.key());
        if(i.value().type() == QVariant::Map)
            addNode(doc,node,i.value().toMap());
        else {
            QDomText text = doc.createTextNode(i.value().toString());
            node.appendChild(text);
        }
        parent.appendChild(node);
    }
}

QVariantMap QSoapClient::getNode(QDomNode &root)
{
    QVariantMap map;
    for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling()) {
        QString name = node.localName();
        if(node.firstChild().isText()) {
            map.insert(name,node.toElement().text());
        } else {
            map.insert(name, getNode(node));
        }
    }
    return map;
}

void QSoapClient::parseError(QDomNode root)
{
    QString errorCode;
    QString errorSubCode;
    QString errorReason;

    for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if(node.localName() == "Code") {
            QDomNode value = findNode(node,"Value");
            if(!value.isNull())
                errorCode = value.toElement().text();
            value = findNode(node,"Subcode");
            if(!value.isNull()) {
                value = findNode(value,"value");
                if(!value.isNull())
                    errorSubCode = value.toElement().text();
            }
        } else if(node.localName() == "Reason") {
            QDomNode value = findNode(node,"Text");
            if(!value.isNull()) {
                errorReason = value.toElement().text();
            }
        }
    }
    emit error("Soap error (code: " + errorCode + (errorSubCode.isEmpty() ? "" : ",subcode: " + errorSubCode) + ",reason: " + errorReason + ")");
}

QDomNode QSoapClient::findNode(QDomNode &parent, const QString &name)
{
    for(QDomNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if(node.localName() == name)
            return node;
    }
    return QDomNode();
}

void QSoapClient::finished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError || reply->error() == QNetworkReply::InternalServerError)
        readMessage(reply->readAll());
    else
        emit error(reply->errorString());
    reply->deleteLater();
}

void QSoapClient::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply)
    authenticator->setUser(p_user);
    authenticator->setPassword(p_password);
}


