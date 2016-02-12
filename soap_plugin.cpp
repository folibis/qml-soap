#include "soap_plugin.h"
#include <qqml.h>
#include "qsoapclient.h"


void SoapPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QSoapClient>(uri, 1, 0, "SoapClient");
}


