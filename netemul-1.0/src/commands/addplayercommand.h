#ifndef ADDPLAYERCOMMAND_H
#define ADDPLAYERCOMMAND_H

#include <QUndoCommand>
#include <QPointF>

class myCanvas;
class DinastyaPlayer;

class addPlayerCommand : public QUndoCommand
{
public:
    addPlayerCommand(myCanvas *scene, QPointF point);
    ~addPlayerCommand();
    void undo();
    void redo();
private:
    myCanvas *scene;
    DinastyaPlayer *player;
    QPointF point;
    bool isOnScene;
};

#endif // ADDPLAYERCOMMAND_H
