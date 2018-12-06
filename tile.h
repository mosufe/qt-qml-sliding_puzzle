#ifndef TILE_H
#define TILE_H
#include <QString>
#include <QObject>

class Tile: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString value READ getValue WRITE writeValue NOTIFY changedValue)
    Q_PROPERTY(QString tileIndex READ getTileIndex WRITE writeTileIndex NOTIFY changedTileIndex)

public:
    explicit Tile(QObject *parent = 0);
    QString getValue();
    void writeValue(QString newValue);
    int getTileIndex();
    void writeTileIndex(int newIndex);

private:
    QString value;
    int tileIndex;

signals:
    void changedValue();
    void changedTileIndex();


};

#endif // TILE_H
