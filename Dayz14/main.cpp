#include <QDebug>
#include <QPoint>
#include <QPolygon>
#include <QtCore>
#include <QtMath>

void drawMap(QList<QPolygon> rockFormations, QPoint start) {
    // Get the size of the map
    QRect map(start, -start);
    for(const auto& formation: rockFormations)
    {
        auto xLimit = std::ranges::minmax_element(formation.constBegin(), formation.constEnd(), [](const QPoint& a, const QPoint& b) { return a.x() < b.x(); });
        auto yLimit = std::ranges::minmax_element(formation.constBegin(), formation.constEnd(), [](const QPoint& a, const QPoint& b) { return a.y() < b.y(); });

        if(map.x() > xLimit.min->x())
        {
            map.setX(xLimit.min->x());
        }
        if(map.y() > yLimit.min->y())
        {
            map.setY(yLimit.min->y());
        }
        if(map.right() < xLimit.max->x())
        {
            map.setRight(xLimit.max->x());
        }
        if(map.bottom() < yLimit.max->y())
        {
            map.setBottom(yLimit.max->y());
        }
    }

    // Create a map with air
    QList<QList<char>> mapData(map.width(), QList<char>(map.height(), '.'));

    // Draw the start
    mapData[start.y() - map.y()][start.x() - map.x()] = '+';
    

    // Draw the formations in the map
    for(const auto& formation: rockFormations)
    {
        for(auto pointIt = formation.constBegin(); pointIt != std::prev(formation.constEnd()); pointIt++)
        {
            // Get the direction
            int xDirection = 0;
            if((*pointIt).x() < (*std::next(pointIt)).x())
            {
                xDirection = 1;
            }
            else if((*pointIt).x() > (*std::next(pointIt)).x())
            {
                xDirection = -1;
            }

            int yDirection = 0;
            if((*pointIt).y() < (*std::next(pointIt)).y())
            {
                yDirection = 1;
            }
            else if((*pointIt).y() > (*std::next(pointIt)).y())
            {
                yDirection = -1;
            }

            // Mark all the points between start and end
            auto pointCount = (*pointIt - (*std::next(pointIt))).manhattanLength();
            for(int i = 0; i < pointCount; i++)
            {
                int y = (*pointIt).y() - map.y() + (i * yDirection);
                int x = (*pointIt).x() - map.x() + (i * xDirection);

                // Set data
                mapData[(*pointIt).y() - map.y() + (i * yDirection)][(*pointIt).x() - map.x() + (i * xDirection)] = '#';
            }
        }
    }

    // Print map
    for(const auto& line: mapData)
    {
        QString mapString;
        for(const auto& point: line)
        {
            mapString.append(point);
        }

        qDebug().noquote() << mapString;
    }
}

int main(int argc, char* argv[]) {
    // Get input file
    // QFile inputFile = QFile("input.txt");
    QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
        return 1;
    }

    // Read map
    QList<QPolygon> rockFormations;
    while(!inputFile.atEnd())
    {
        QString line = inputFile.readLine().trimmed();
        if(line == "")
        {
            continue;
        }

        // Extract coordinates
        auto          coordinates = line.split("->");
        QList<QPoint> rockPoints;
        for(const auto& point: coordinates)
        {
            // Split into x and y
            auto xyValue = point.trimmed().split(',');

            // Sanity check
            if(xyValue.count() != 2)
            {
                qDebug() << "Invalid input";
                return 1;
            }

            // Create point
            rockPoints.append({xyValue.first().toInt(), xyValue.last().toInt()});
        }

        // Create polygon
        rockFormations.append(QPolygon(rockPoints));
    }


    drawMap(rockFormations, {500, 0});
    return 0;
}