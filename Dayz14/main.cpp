#include <QDebug>
#include <QPoint>
#include <QPolygon>
#include <QtCore>
#include <QtMath>

#define part_2

struct Map {
        QPoint             start;
        QRect              field;
        QList<QList<char>> mapData;

        // Calculating functions
        int maxSteps() const {
            return QPoint(field.width(), field.height()).manhattanLength();
        }
};

bool placeRock(Map& map, QPoint rock, bool testOnly = false) {
    auto& mapData = map.mapData;
    auto& field   = map.field;

    // Sanity check
    if(!field.contains(rock))
    {
        if(!testOnly)
        {
            qWarning() << "Rock falls of map!";
        }
        return true;
    }

    auto& current = mapData[rock.y() - field.y()][rock.x() - field.x()];

    if(current == '#')
    {
        if(!testOnly)
        {
            qWarning() << "Rock placed on top of a wall!";
            mapData[rock.y() - field.y()][rock.x() - field.x()] = '$';
        }
        return false;
    }
    else if(current == 'O')
    {
        if(!testOnly)
        {
            qWarning() << "Rock placed on top of a rock!";
            mapData[rock.y() - field.y()][rock.x() - field.x()] = '8';
        }
        return false;
    }
    else
    {
        if(!testOnly)
        {
            mapData[rock.y() - field.y()][rock.x() - field.x()] = 'O';
        }
        return true;
    }
}

bool doStep(Map& map, int steps = 0) {
    // Create a new rock
    auto newRock  = map.start;

    bool movement = false;
    do {
        // Check down
        if(movement = placeRock(map, newRock + QPoint(0, 1), true), movement)
        {
            newRock += QPoint(0, 1);
        }
        // Check left
        else if(movement = placeRock(map, newRock + QPoint(-1, 1), true), movement)
        {
            newRock += QPoint(-1, 1);
        }
        // Check right
        else if(movement = placeRock(map, newRock + QPoint(1, 1), true), movement)
        {
            newRock += QPoint(1, 1);
        }

        // Check if fell of the map
        if(!map.field.contains(newRock))
        {
            return false;
        }

    } while(movement && --steps != 0);

    // Should always be true, but informs the user if it's false
    placeRock(map, newRock);

    // Return false if we are full
    return (newRock != map.start);
}

Map createMap(QPoint start, QList<QPolygon> rockFormations, QList<QPoint> rocks = QList<QPoint>()) {
    // Get the size of the map
    QRect field(start, -start);
    for(const auto& formation: rockFormations)
    {
        auto xLimit = std::ranges::minmax_element(formation.constBegin(), formation.constEnd(), [](const QPoint& a, const QPoint& b) { return a.x() < b.x(); });
        auto yLimit = std::ranges::minmax_element(formation.constBegin(), formation.constEnd(), [](const QPoint& a, const QPoint& b) { return a.y() < b.y(); });

        if(field.x() > xLimit.min->x())
        {
            field.setX(xLimit.min->x());
        }
        if(field.y() > yLimit.min->y())
        {
            field.setY(yLimit.min->y());
        }
        if(field.right() < xLimit.max->x())
        {
            field.setRight(xLimit.max->x());
        }
        if(field.bottom() < yLimit.max->y())
        {
            field.setBottom(yLimit.max->y());
        }
    }

    // Create a map with air
    QList<QList<char>> mapData(field.height(), QList<char>(field.width(), '.'));

    // Draw the start
    mapData[start.y() - field.y()][start.x() - field.x()] = '+';

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
            auto pointCount = (*pointIt - (*std::next(pointIt))).manhattanLength() + 1;
            for(int i = 0; i < pointCount; i++)
            {
                auto y = (*pointIt).y() - field.y() + (i * yDirection);
                auto x = (*pointIt).x() - field.x() + (i * xDirection);

                // Set
                mapData[y][x] = '#';
            }
        }
    }

    // Place the rocks
    for(const auto& rock: rocks)
    {
        // Sanity check
        if(mapData[rock.y() - field.y()][rock.x() - field.x()] == '#')
        {
            qWarning() << "Rock placed on top of a wall!";
            mapData[rock.y() - field.y()][rock.x() - field.x()] = '8';
        }
        else
        {
            mapData[rock.y() - field.y()][rock.x() - field.x()] = 'O';
        }
    }

    return {start, field, mapData};
}

void drawMap(Map map) {
    // Print map
    for(const auto& line: map.mapData)
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
     QFile inputFile = QFile("input.txt");
    //QFile inputFile = QFile("testInput.txt");
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

#ifdef part_2
    // Find the lowest point
    auto tempMap    = createMap({500, 0}, rockFormations);
    auto floorLevel = tempMap.field.bottom() + 2;

    // Add a floor
    int floorSize = tempMap.field.height();
    rockFormations.append(QPolygon({{tempMap.field.left() - floorSize, floorLevel}, {tempMap.field.right() + floorSize, floorLevel}}));
#endif

    auto map   = createMap({500, 0}, rockFormations);

    int  steps = 0;
    while(doStep(map, map.maxSteps()))
    {
        // drawMap(map);
        steps++;
    }

#ifdef part_2
    // Don't forget the last grain of sand
    steps += 1;
#endif

    qDebug() << "\nFinal result:";
    drawMap(map);

    qDebug() << "The map is full in" << steps << "steps";

    return 0;
}