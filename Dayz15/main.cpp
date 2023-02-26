#include <QDebug>
#include <QPoint>
#include <QPolygon>
#include <QtCore>
#include <QtMath>

class Sensor : public QPoint {
    public:
        Sensor(int x, int y, int b_x, int b_y)
              : QPoint(x, y), closestBeacon(b_x, b_y){};

        int beaconDistance() const {
            return (QPoint(*this) - closestBeacon).manhattanLength();
        }

        QPoint beacon() const {
            return closestBeacon;
        }

    private:
        QPoint closestBeacon;
};

struct Map {
        QRect              field;
        QList<QList<char>> mapData;

        // Calculating functions
        int maxSteps() const {
            return QPoint(field.width(), field.height()).manhattanLength();
        }
};

Map createMap(QList<Sensor> sensors) {
    // Get the size of the map
    QRect field;
    int   maxDistance = 0;
    for(const auto& sensor: sensors)
    {
        maxDistance = std::max(maxDistance, std::ranges::max(sensors, {}, &Sensor::beaconDistance).beaconDistance());

        if(field.x() > sensor.x())
        {
            field.setX(sensor.x());
        }
        if(field.y() > sensor.y())
        {
            field.setY(sensor.y());
        }
        if(field.right() < sensor.x())
        {
            field.setRight(sensor.x());
        }
        if(field.bottom() < sensor.y())
        {
            field.setBottom(sensor.y());
        }
    }

    // Extend with range
    field.setX(field.x() - maxDistance);
    field.setY(field.y() - maxDistance);
    field.setRight(field.right() + maxDistance);
    field.setBottom(field.bottom() + maxDistance);

    // Create a map with air
    QList<QList<char>> mapData(field.height(), QList<char>(field.width(), '.'));

    // Draw the sensor ranges in the map
    /*for(const auto& sensor: sensors)
    {
        for(int y = sensor.y() - sensor.beaconDistance(); y <= sensor.y() + sensor.beaconDistance(); y++)
        {
            for(int x = sensor.x() - sensor.beaconDistance(); x <= sensor.x() + sensor.beaconDistance(); x++)
            {
                if(abs((QPoint(x, y) - sensor).manhattanLength()) <= sensor.beaconDistance())
                {
                    mapData[y - field.y()][x - field.x()] = '#';
                }
            }
        }
    }*/

    // Draw the sensors and beacons in the map
    for(const auto& sensor: sensors)
    {
        mapData[sensor.y() - field.y()][sensor.x() - field.x()]                   = 'S';
        mapData[sensor.beacon().y() - field.y()][sensor.beacon().x() - field.x()] = 'B';
    }

    return {field, mapData};
}

void drawMap(QList<QList<char>> mapData) {
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

void drawMap(Map map) {
    drawMap(map.mapData);
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
    QList<Sensor> sensors;
    while(!inputFile.atEnd())
    {
        QString line = inputFile.readLine().trimmed();
        if(line == "")
        {
            continue;
        }

        // Extract information
        auto searchCriteria = QRegularExpression(R"([xy]=(-?\d+))");
        auto matches        = searchCriteria.globalMatch(line);

        // Extract sensor
        auto sensorX = matches.next().captured(1).toInt();
        auto sensorY = matches.next().captured(1).toInt();

        // Extract beacon info
        auto beaconX = matches.next().captured(1).toInt();
        auto beaconY = matches.next().captured(1).toInt();

        // Append the sensor
        sensors.append({sensorX, sensorY, beaconX, beaconY});
    }

    // Create a map for the sensors
    auto map = createMap(sensors);

    // Print info about the map
    qDebug() << "Field size:" << map.field.size();

    // Print map
    //drawMap(map);

    // Get the "No beacon position"-count
    int  targetRow   = 2000000;
    auto nopeSitions = std::count_if(map.mapData[targetRow - map.field.top()].constBegin(), map.mapData[targetRow - map.field.top()].constEnd(), [](const char x) { return x == '#'; });
    qDebug() << "Found count:" << nopeSitions;

    return 0;
}