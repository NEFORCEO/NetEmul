#include "scenexmlwriter.h"
#include "mycanvas.h"
#include "device.h"
#include "dinastyaplayer.h"

sceneXmlWriter::sceneXmlWriter(myCanvas *scene) : myScene(scene)
{
    setAutoFormatting(true);
}

void sceneXmlWriter::writeScene(QIODevice *dev)
{
    setDevice(dev);
    writeStartDocument();
    writeStartElement("netemul");
    writeAttribute("version",QCoreApplication::applicationVersion() );
    foreach ( device::device *i , myScene->devices() )
        i->writeXml(*this);
    foreach ( textItem *i , myScene->textItems() ) {
        writeStartElement("textitem");
        writeAttribute("x" , QString::number( i->pos().x() ) );
        writeAttribute("y", QString::number( i->pos().y() ) );
        writeCharacters( i->toPlainText() );
        writeEndElement();
    }
    foreach ( DinastyaPlayer *i, myScene->players() ) {
        writeStartElement("dinastyaplayer");
        writeAttribute("x", QString::number(i->pos().x()));
        writeAttribute("y", QString::number(i->pos().y()));
        writeEndElement();
    }
    writeEndDocument();
}
