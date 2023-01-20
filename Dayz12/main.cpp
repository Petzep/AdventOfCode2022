#include <QDebug>
#include <QPoint>
#include <QtCore>
#include <QtMath>

class ForestMap {
        Q_GADGET

    public:
        ForestMap(QStringList input) {
            if(!loadMap(input))
            {
                qDebug() << "Map is not loaded correctly";
            }
        }

        ~ForestMap() {
            clearMap();
        }

        bool loadMap(QStringList map) {
            // Clear map if needed
            if(m_map)
            {
                clearMap();
            }

            // Parse size from map
            m_y = map.size();
            m_x = map.at(0).size();

            // allocate memory for the map
            m_map = new uint8_t*[m_y];
            for(size_t i = 0; i < m_y; i++)
            {
                m_map[i] = new uint8_t[m_x];
            }

            // Parse map
            int locationsSet = 2;
            for(int y = 0; y < m_y; y++)
            {
                for(int x = 0; x < m_x; x++)
                {
                    // Filter start location
                    if(map.at(y).at(x) == 'S')
                    {
                        m_startLocation = QPoint(x, y);
                        m_map[y][x]     = QChar('a').toLatin1() - QChar('a').toLatin1();
                        locationsSet--;
                    }
                    // Filter end location
                    else if(map.at(y).at(x) == 'E')
                    {
                        m_endLocation = QPoint(x, y);
                        m_map[y][x]   = QChar('z').toLatin1() - QChar('a').toLatin1();
                        locationsSet--;
                    }
                    // Input height
                    else
                    {
                        m_map[y][x] = map.at(y).at(x).toLatin1() - QChar('a').toLatin1();
                    }
                }
            }

            // Check if start and end location are set
            return !locationsSet;
        }

        // A* finds a path from start to goal.
        // Based on pseudo code of: https://en.wikipedia.org/wiki/A*_search_algorithm
        int findPath(uint8_t goalElevation = -1) {
            uint8_t limitElevation = 0;
            // The set of discovered nodes that may need to be (re-)expanded.
            // Initially, only the start node is known.
            // This is usually implemented as a min-heap or priority queue rather than a hash-set.
            QList<QPoint> openList;
            openList.append(m_startLocation);

            // For node n, cameFrom[n] is the node immediately preceding it on the cheapest path from start
            // to n currently known.
            QMap<int, QPoint> cameFrom;

            // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
            // For node n, fScore[n] := gScore[n] + h(n). fScore[n] represents our current best guess as to
            // how short a path from start to finish can be if it goes through n.
            auto gScore = QMap<int, double>();
            auto fScore = QMap<int, double>();
            for(int i = 0; i < m_x * m_y; i++)
            {
                gScore[i] = m_x * m_y * m_map[m_endLocation.y()][m_endLocation.x()];
                fScore[i] = m_x * m_y * m_map[m_endLocation.y()][m_endLocation.x()];
            }
            gScore[point2Int(m_startLocation)] = 0;
            fScore[point2Int(m_startLocation)] = costToGoal(m_startLocation);


            while(!openList.isEmpty())
            {
                // This operation can occur in O(Log(N)) time if openSet is a min-heap or a priority queue
                // But I'm lazy, so I'll just sort the list in the most ineffective way (famous last words)
                std::sort(openList.begin(), openList.end(), [this](QPoint a, QPoint b) {
                    return costToGoal(a) < costToGoal(b);
                });

                auto current = openList.takeLast();
                if(current == m_endLocation)
                {
                    m_path = reconstructPath(current, cameFrom);
                    return m_map[m_endLocation.y()][m_endLocation.x()];
                }
                else if(m_map[current.y()][current.x()] == goalElevation)
                {
                    m_path = reconstructPath(current, cameFrom);
                    return goalElevation;
                }

                if(goalElevation > m_map[m_startLocation.y()][m_startLocation.x()])
                {
                    limitElevation = std::max(limitElevation, m_map[current.y()][current.x()]);
                }
                else
                {
                    limitElevation = std::min(limitElevation, m_map[current.y()][current.x()]);
                }

                // Search neighbors
                QList<QPoint> neighbors = findNeighbours(current);

                // Debug
                /*qDebug() << QString("Current(x:%1,y:%2) - Neighbors: %3").arg(current.x()).arg(current.y()).arg(neighbors.size());
                for(const auto& neighbor: neighbors)
                {
                    qDebug() << QString("(%1,%2)").arg(neighbor.x()).arg(neighbor.y());
                }
                //if(point2Int(current) == point2Int({171, 28}))
                if(m_map[current.y()][current.x()] == 'n' - 'a')
                {
                    printMap(current, cameFrom);
                    //return false;
                    __debugbreak();
                }
                */

                // For all neighbors
                for(const auto& neighbor: neighbors)
                {
                    // d(current,neighbor) is the weight of the edge from current to neighbor
                    // tentative_gScore is the distance from start to the neighbor through current
                    auto tentative_gScore = gScore[point2Int(current)] + costTo(current, neighbor);

                    if(tentative_gScore < gScore[point2Int(neighbor)])
                    {
                        // This path to neighbor is better than any previous one. Record it!
                        cameFrom[point2Int(neighbor)] = current;
                        gScore[point2Int(neighbor)]   = tentative_gScore;
                        fScore[point2Int(neighbor)]   = tentative_gScore + costToGoal(neighbor);

                        if(!openList.contains(neighbor))
                        {
                            openList.append(neighbor);
                        }
                    }
                }
            }

            // Open set is empty but goal was never reached
            // No path found, return highest elevation
            return findPath(limitElevation);
        }

        int findPathStart() {
            QList<int> elevationMap;

            // I'm to lazy, there is a stroke of b elevation at the second column, meaning. All possible candidates are in the first column. Lets go from there.
            for(int y = 0; y < m_y; y++)
            {
                m_startLocation.setY(y);
                m_path.clear();

                // Looking at the map, there is always a path possible. Trust in my code and go
                findPath();
                elevationMap.append(m_path.count());
            }

            // Find the minimum
            return std::ranges::min(elevationMap);
        }

        QPoint startLocation() const {
            return m_startLocation;
        }

        QPoint endLocation() const {
            return m_endLocation;
        }

        QList<QPoint> path() const {
            return m_path;
        }

        void printMap(QPoint current, QMap<int, QPoint> cameFrom) const {
            auto path = reconstructPath(current, cameFrom);
            printMap(path);
        }

        void printMap(QList<QPoint> path) const {
            for(int y = 0; y < m_y; y++)
            {
                QString line;
                for(int x = 0; x < m_x; x++)
                {
                    if(m_startLocation == QPoint(x, y))
                    {
                        line.append("S");
                    }
                    else if(m_endLocation == QPoint(x, y))
                    {
                        line.append("E");
                    }
                    else if(path.contains(QPoint(x, y)))
                    {
                        line.append("*");
                    }
                    else
                    {
                        if(costTo({x, y}, path.first()) < 5)
                        {
                            line.append((char)('a' + m_map[y][x]));
                        }
                        else
                        {
                            line.append(" ");
                        }
                    }
                }

                qDebug() << line;
            }
        }

        explicit operator bool() const {
            return m_map;
        }

    private:
        uint8_t**     m_map = nullptr;
        size_t        m_y;
        size_t        m_x;

        QPoint        m_startLocation;
        QPoint        m_endLocation;

        QList<QPoint> m_path;

        // Functions
        void clearMap() {
            // free memory
            for(size_t i = 0; i < m_y; ++i)
            {
                delete[] m_map[i];
            }
            delete[] m_map;
        }

        int point2Int(const QPoint& p) const {
            return p.y() * m_x + p.x();
        }

        QList<QPoint> reconstructPath(QPoint current, QMap<int, QPoint> cameFrom) const {
            // Reconstruct the path
            QList<QPoint> path;
            while(current != m_startLocation)
            {
                path.append(current);
                current = cameFrom[point2Int(current)];
            }
            path.append(m_startLocation);

            return path;
        }

        double costTo(QPoint current, QPoint target) const {
            // Manhattan distance (no height)
            // auto diffPoint = target - current;
            // return sqrt(pow(diffPoint.manhattanLength(), 2) + pow(diffPoint.manhattanLength(), 2));

            // Manhattan distance
            // auto diffPoint = target - current;
            // return sqrt(pow(diffPoint.manhattanLength(), 2) + pow(diffPoint.manhattanLength(), 2) + pow(m_map[target.y()][target.x()] - m_map[current.y()][current.x()], 2));

            // Height as difference
            // return m_map[target.y()][target.x()] - m_map[current.y()][current.x()];

            // Pythagorean distance
            return sqrt(pow(target.x() - current.x(), 2) + pow(target.y() - current.y(), 2) + pow(m_map[target.y()][target.x()] - m_map[current.y()][current.x()], 2));
        }

        double costToGoal(QPoint current) const {
            return costTo(current, m_endLocation);
        }

        QList<QPoint> findNeighbours(QPoint current) {
            QList<QPoint> neighbors = QList<QPoint>();

            // Top neighbor
            if(current.y() > 0 && (m_map[current.y() - 1][current.x()] - m_map[current.y()][current.x()]) <= 1)
            {
                neighbors.append({current.x(), current.y() - 1});
            }

            // Bottom neighbor
            if(current.y() < m_y - 1 && (m_map[current.y() + 1][current.x()] - m_map[current.y()][current.x()]) <= 1)
            {
                neighbors.append({current.x(), current.y() + 1});
            }

            // Left neighbor
            if(current.x() > 0 && (m_map[current.y()][current.x() - 1] - m_map[current.y()][current.x()]) <= 1)
            {
                neighbors.append({current.x() - 1, current.y()});
            }

            // Right neighbor
            if(current.x() < m_x - 1 && (m_map[current.y()][current.x() + 1] - m_map[current.y()][current.x()]) <= 1)
            {
                neighbors.append({current.x() + 1, current.y()});
            }

            return neighbors;
        }
};

int main(int argc, char* argv[]) {
    // Get input file
    QFile inputFile = QFile("input.txt");
    // QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
        return 1;
    }

    // Read map
    QStringList mapInput;
    while(!inputFile.atEnd())
    {
        QString inputLine = inputFile.readLine().trimmed();
        mapInput.append(inputLine);
    }
    ForestMap* forest = new ForestMap(mapInput);

    // Output info
    qDebug() << "Start location:" << forest->startLocation();
    qDebug() << "End location:" << forest->endLocation();

    // Part 1
    if(forest->findPath())
    {
        qDebug() << "Found path!";
        auto foundPath = forest->path();

        // Check if the found path leads to the end
        if(foundPath.first() == forest->endLocation())
        {
            qDebug() << "Found lead to the end!";
        }

        forest->printMap(foundPath);
        QStringList pathList;
        for(const auto& point: foundPath)
        {
            pathList.append(QString("(%1,%2)").arg(point.x()).arg(point.y()));
        }
        // qDebug() << pathList.join(" -> ");

        qDebug() << "Steps to take:" << forest->path().size() - 1;
    }
    else
    {
        qDebug() << "No path found :-(";
    }

    // Part 2
    // Ballsy, no checking here!
    qDebug() << "Finding shortest path!";
    qDebug() << "Steps to take:" << forest->findPathStart() - 1;


    return 0;
}