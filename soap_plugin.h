#ifndef SOAP_PLUGIN_H
#define SOAP_PLUGIN_H

#include <QQmlExtensionPlugin>

class SoapPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Soap")

public:
    void registerTypes(const char *uri);
};

#endif

