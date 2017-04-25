/*
 * UFO Detector | www.UFOID.net
 *
 * Copyright (C) 2016 UFOID
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef POLYGONNODE_H
#define POLYGONNODE_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QDebug>

class PolygonEdge;


class PolygonNode : public QGraphicsEllipseItem
{
public:

    enum { Type = QGraphicsItem::UserType + 1 };

    PolygonNode(QGraphicsItem *parent = 0);
    ~PolygonNode();

    /**
     * @brief Add an edge which is connected to this node.
     * @param edge
     */
    void addEdge(PolygonEdge* edge);

#ifndef _UNIT_TEST_
private:
#endif
    QVector<PolygonEdge*> m_connectedEdges;  ///< edges connected to this node

protected:
    int type() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // POLYGONNODE_H
