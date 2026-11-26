/****************************************************************************************
** NetEmul - program for simulating computer networks.
** Copyright © 2009 Semenov Pavel and Omilaeva Anastasia
**
** NetEmul is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** NetEmul is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with the NetEmul; if not, write to the Free
** Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
** 02111-1307 USA.
****************************************************************************************/
#include "deletecommand.h"
#include "mycanvas.h"
#include "cabledev.h"
#include "device.h"
#include "textitem.h"
#include "dinastyaplayer.h"

deleteCommand::deleteCommand(myCanvas *s, QList<QGraphicsItem*> list)
    : scene(s), isOnScene(true)
{
    foreach ( QGraphicsItem *i, list) {
        if ( scene->isDevice(i) ) {
            device *dev = qgraphicsitem_cast<device*>(i);
            extractDevices << dev;
            foreach ( cableDev *j, dev->cables() ) extractCables << j;
        }
        else if ( i->type() == cableDev::Type ) extractCables << qgraphicsitem_cast<cableDev*>(i);
        else if (i->type() == textItem::Type )  extractText << qgraphicsitem_cast<textItem*>(i);
        else if (i->type() == DinastyaPlayer::Type )
            extractPlayers << static_cast<DinastyaPlayer*>(i);
    }
    setText(QObject::tr("Delete"));
}

deleteCommand::~deleteCommand()
{
    if ( isOnScene ) return;
    qDeleteAll(extractDevices);
    qDeleteAll(extractCables);
    qDeleteAll(extractText);
    qDeleteAll(extractPlayers);
}

void deleteCommand::undo()
{
    foreach ( device *i, extractDevices ) scene->registerDevice(i);
    foreach ( cableDev *i, extractCables ) createConnection(i);
    foreach ( textItem *i , extractText ) scene->registerText(i);
    foreach ( DinastyaPlayer *i , extractPlayers ) scene->registerPlayer(i);
    isOnScene = true;
}

void deleteCommand::redo()
{
    foreach ( device *i, extractDevices ) scene->unregisterDevice(i);
    foreach ( cableDev *i, extractCables ) deleteConnection(i);
    foreach ( textItem *i, extractText ) scene->unregisterText(i);
    foreach ( DinastyaPlayer *i, extractPlayers ) scene->unregisterPlayer(i);
    isOnScene = false;
}

void deleteCommand::deleteConnection(cableDev *cable)
{
    cable->unregisterCable();
    scene->unregisterCable(cable);
}

void deleteCommand::createConnection(cableDev *cable)
{
    cable->registerCable();
    scene->registerCable(cable);
}
