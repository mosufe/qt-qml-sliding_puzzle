#include "tile.h"

Tile::Tile(QObject *parent): QObject(parent)
{
    value = "P";
    tileIndex = 0;
}

QString Tile::getValue()
{
    return value;
}

void Tile::writeValue(QString newValue)
{
    value = newValue;
}

int Tile::getTileIndex()
{
    return tileIndex;
}

void Tile::writeTileIndex(int newIndex)
{
    tileIndex = newIndex;
}
