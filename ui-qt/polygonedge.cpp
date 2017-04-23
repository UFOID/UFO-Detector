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

#include "polygonedge.h"

PolygonEdge::PolygonEdge(PolygonNode* startNode, PolygonNode* endNode,
    QGraphicsLineItem* parent) : QGraphicsLineItem(parent)
{
    setPen(QPen(Qt::red));

    if (startNode && endNode) {
        m_startNode = startNode;
        m_endNode = endNode;
        m_startNode->addEdge(this);
        m_endNode->addEdge(this);
        setLine(m_startNode->x(), m_startNode->y(), m_endNode->x(), m_endNode->y());
    } else {
        m_startNode = NULL;
        m_endNode = NULL;
    }
}

PolygonEdge::~PolygonEdge() {
    m_startNode = NULL;
    m_endNode = NULL;
}

void PolygonEdge::notifyNodeChange() {
    if (m_startNode && m_endNode) {
        setLine(m_startNode->x(), m_startNode->y(), m_endNode->x(), m_endNode->y());
    }
}

int PolygonEdge::type() const {
    return PolygonEdge::Type;
}
