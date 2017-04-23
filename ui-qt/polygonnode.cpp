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

#include "polygonnode.h"
#include "polygonedge.h"

PolygonNode::PolygonNode(QGraphicsItem* parent) : QGraphicsEllipseItem(parent)
{
    setFlag(GraphicsItemFlag::ItemIsSelectable);
    setFlag(GraphicsItemFlag::ItemIsMovable);
    setFlag(GraphicsItemFlag::ItemSendsGeometryChanges);
    int radius = 4;
    setRect(-radius, -radius, radius*2, radius*2);
    setPen(QPen(Qt::gray));
    setBrush(QBrush(Qt::black));
}

PolygonNode::~PolygonNode() {
    m_connectedEdges.clear();
}

void PolygonNode::addEdge(PolygonEdge* edge) {
    if (edge) {
        m_connectedEdges.append(edge);
    }
}

int PolygonNode::type() const {
    return PolygonNode::Type;
}

QVariant PolygonNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (QGraphicsItem::ItemPositionHasChanged == change) {
        foreach (PolygonEdge* edge, m_connectedEdges) {
            edge->notifyNodeChange();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
