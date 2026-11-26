#include "addplayercommand.h"
#include "mycanvas.h"
#include "dinastyaplayer.h"

addPlayerCommand::addPlayerCommand(myCanvas *s, QPointF p)
    : scene(s), player(0), point(p), isOnScene(false)
{
    setText(QObject::tr("Add Dinastya player"));
}

addPlayerCommand::~addPlayerCommand()
{
    if (!isOnScene)
        delete player;
}

void addPlayerCommand::undo()
{
    scene->unregisterPlayer(player);
    isOnScene = false;
}

void addPlayerCommand::redo()
{
    if (!player)
        player = scene->createDinastyaPlayer(point);
    else
        scene->registerPlayer(player);
    isOnScene = true;
}
