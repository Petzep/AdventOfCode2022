#include <QtCore>

class ForrestMap {
    public:
        ForrestMap(QStringList input)
              : ForrestMap(input.size(), input.at(0).size()) {
            loadMap(input);
        }

        ForrestMap(size_t x, size_t y)
              : m_x(x), m_y(y) {
            
            // allocate memory for the map
            m_map = new int*[x];
            for(size_t i = 0; i < x; ++i)
            {
                m_map[i] = new int[y];
            }
        }

        ~ForrestMap() {
            // free memory
            for(size_t i = 0; i < m_x; ++i)
            {
                delete[] m_map[i];
            }
            delete[] m_map;
        }

        bool loadMap(QStringList map) {
        }

        QPoint startLocation() const {
            return m_startLocation;
        }

        QPoint endLocation() const {
            return m_endLocation;
        }

    private:
        int**         m_map;
        size_t        m_x;
        size_t        m_y;

        QPoint        m_startLocation;
        QPoint        m_endLocation;

        QList<QPoint> m_path;
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
    QStringList mapInput;
    while(!inputFile.atEnd())
    {
        QString inputLine = inputFile.readLine().trimmed();
        mapInput.append(inputLine);
    }

    ForrestMap(ma)

    return 0;
}