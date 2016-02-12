# qml-soap

qml-soap is simple SOAP client plugin created to use mostly with QML.
It can be used in 2 ways:

1. as a standalone QML plugin
2. as a static QML extension

## Installation
**Standalone QML plugin**

1: Download the [source](https://github.com/folibis/qml-soap.git) from Github into your source directory:
 
```sh
git clone https://github.com/folibis/qml-soap.git
```

2: Open the project with QtCreator or from command line and compile the source.

3: Deploy the plugin. It will be installed into Qt system folder as a QML plugin

**Static QML extension**

1: Download the [source](https://github.com/folibis/qml-soap.git) from Github into your project directory:

```sh
cd project_folder
git clone https://github.com/folibis/qml-soap.git
```
It will be downloaded as `qml-soap` folder inside your project

2: add following line into your project *.pro file:
 
```text
include($$PWD/qml-soap/soap_static.pri)
```

3: Add folowing lines into main.cpp file:

```cpp
#include "soap_plugin.h"
...
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    SoapPlugin plugin;
    plugin.registerTypes("Soap");
    ...
}
```
4: rebuild the project

## Using

A simple usage example of the plugin to create length unit converter using [webservicex.com](http://www.webservicex.com/New/Home/ServiceDetail/21) services over SOAP:

```javascript
import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Soap 1.0

Window {
    visible: true
    width: 700
    height: 200

    RowLayout {
        spacing: 10
        anchors.centerIn: parent
        Text { text: "Convert" }
        TextField { id: value; text: "1" }
        ComboBox {
            id: fromUnit
            model: ["Angstroms","Nanometers","Microinch","Microns","Mils","Millimeters","Centimeters","Inches","Links","Spans","Feet","Cubits","Varas","Yards","Meters","Fathoms","Rods","Chains","Furlongs","Cablelengths","Miles","Kilometers","Nauticalmile","League","Nauticalleague"]
            Layout.preferredWidth: 100
        }
        Text { text: "to" }
        ComboBox {
            id: toUnit
            model: fromUnit.model
            Layout.preferredWidth: 100
        }
        Button {
            text: "="
            onClicked: {
                var args = {
                    "LengthValue": value.text,
                    "fromLengthUnit": fromUnit.currentText,
                    "toLengthUnit": toUnit.currentText
                };
                soapClient.call("ChangeLengthUnit",args,"http://www.webserviceX.NET/");
            }
        }
        TextField { id: result; readOnly: true }
    }

    SoapClient {
        id: soapClient
        url: "http://www.webservicex.net/length.asmx?WSDL"
        onMessageReceived: {
            console.log(parseData(data));
            result.text = data.ChangeLengthUnitResponse.ChangeLengthUnitResult;
            result.cursorPosition = 0;
        }
        onError: {
            console.log(error);
        }

        function parseData(arr,step) {
            step = step || 1;
            var padding = new Array(step * 4 + 1).join(" ");
            var retval = "";

            for (var key in arr) {
                if( Object.prototype.toString.call(arr[key]) === '[object Object]' ) {
                    retval += padding + key + "\n";
                    retval += padding + parseData(arr[key],step + 1);
                } else {
                    retval += padding + key + ": " + arr[key] + "\n";
                }
            }
            return retval;
        }
    }
}
```








