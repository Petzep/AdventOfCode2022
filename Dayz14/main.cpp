#include <QDebug>
#include <QPoint>
#include <QPolygon>
#include <QtCore>
#include <QtMath>

void drawMap(QList<QPolygon> rockFormations, QPoint start) {
    // Get the size of the map
    QRect map(start, -start);
    for(const auto& formation : rockFormations) {
        auto xLimit = std::ranges::minmax_element(formation.constBegin(), formation.constEnd(), [](const QPoint& a, const QPoint& b) { return a.x() < b.x(); });
        auto yLimit = std::ranges::minmax_element(formation.constBegin(), formation.constEnd(), [](const QPoint& a, const QPoint& b) { return a.y() < b.y(); });

        if(map.x() > xLimit.min->x()) {
            map.setX(xLimit.min->x());
        }
        if(map.y() > yLimit.min->y()) {
            map.setY(yLimit.min->y());
        }
        if(map.right() < xLimit.max->x()) {
            map.setRight(xLimit.max->x());
        }
        if(map.bottom() < yLimit.max->y()) {
            map.setBottom(yLimit.max->y());
        }
    }

    // Print map
    

    
}

int main(int argc, char* argv[]) {
    // Get input file
    //QFile inputFile = QFile("input.txt");
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
        auto coordinates = line.split("->");
        QList<QPoint> rockPoints;
        for(const auto& point : coordinates) {
            // Split into x and y
            auto xyValue = point.trimmed().split(',');
            
            // Sanity check
            if(xyValue.count() != 2) {
                qDebug() << "Invalid input";
                return 1;
            }

            // Create point
            rockPoints.append({xyValue.first().toInt(), xyValue.last().toInt()});
        }

        // Create polygon
        rockFormations.append(QPolygon(rockPoints));
    }


    drawMap(rockFormations, { 500, 0 });
    return 0;
}