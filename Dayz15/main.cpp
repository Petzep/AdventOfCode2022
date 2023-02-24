#include <QDebug>
#include <QPoint>
#include <QPolygon>
#include <QtCore>
#include <QtMath>

class Sensor : public QPoint {
    public:
        Sensor(int x, int y, int b_x, int b_y)
              : QPoint(x, y), closestBeacon(b_x, b_y){};
        
        int beaconDistance() {
            return (QPoint(*this) - closestBeacon).manhattanLength();
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

    return 0;
}