#include <QPoint>
#include <QtCore>

class Rope {
    public:
        explicit Rope(size_t size) {
            knots = QList<QPoint>(size - 1, {0, 0});
        }
        void move(Qt::ArrowType direction, int distance = 1) {
            for(int i = 0; i < distance; i++)
            {
                auto oldPosition = head;
                switch(direction)
                {
                    case Qt::UpArrow:
                        head.setY(head.y() + 1);
                        break;
                    case Qt::DownArrow:
                        head.setY(head.y() - 1);
                        break;
                    case Qt::LeftArrow:
                        head.setX(head.x() - 1);
                        break;
                    case Qt::RightArrow:
                        head.setX(head.x() + 1);
                        break;
                }


                auto relativeHead = head;
                for(auto& knot: knots)
                {
                    auto newKnot = knot;
                    // Update tail
                    if(relativeHead.x() == knot.x() || relativeHead.y() == knot.y())
                    {
                        // The tail is still in the same direction as the head
                        if((relativeHead - knot).manhattanLength() > 1)
                        {
                            // Damn, my old hack works here with the short rope (n=2) but not with the longer ropes
                            // newKnot = oldPosition;

                            if(knot.x() < relativeHead.x())
                                newKnot.setX(knot.x() + 1);
                            if(knot.x() > relativeHead.x())
                                newKnot.setX(knot.x() - 1);
                            if(knot.y() < relativeHead.y())
                                newKnot.setY(knot.y() + 1);
                            if(knot.y() > relativeHead.y())
                                newKnot.setY(knot.y() - 1);
                        }
                    }
                    else // Diagonal
                    {
                        if((relativeHead - knot).manhattanLength() > 2)
                        {
                            // This worked with path 1, nice hack
                            // newKnot = oldPosition;

                            // Let's do it correctly
                            // move the knot diagonally closer to the relativeHead
                            if(knot.x() < relativeHead.x())
                                newKnot.setX(knot.x() + 1);
                            if(knot.x() > relativeHead.x())
                                newKnot.setX(knot.x() - 1);
                            if(knot.y() < relativeHead.y())
                                newKnot.setY(knot.y() + 1);
                            if(knot.y() > relativeHead.y())
                                newKnot.setY(knot.y() - 1);
                        }
                    }

                    oldPosition  = knot;
                    knot         = newKnot;
                    relativeHead = newKnot;
                }

                // Print state
                /*
                qDebug() << "-------------------\n";
                printState();
                qDebug() << '\n';
                //*/

                // Do some bookkeeping
                if(!tailHistory.contains(tailPosition()))
                {
                    tailHistory.append(tailPosition());
                }
            }
        }

        QPoint headPosition() {
            return head;
        }

        QPoint tailPosition() {
            return knots.last();
        }

        QList<QPoint> history() {
            return tailHistory;
        }

        void printState() {
            // Find min and maximum points
            //auto min = QPoint(0, 0);
            //auto max = QPoint(6, 6);
            auto min = QPoint(-11, -6);
            auto max = QPoint(14, 16);

            for(auto& knot: knots)
            {
                min.setX(qMin(min.x(), knot.x()));
                min.setY(qMin(min.y(), knot.y()));
                max.setX(qMax(max.x(), knot.x()));
                max.setY(qMax(max.y(), knot.y()));
            }

            // Create a grid
            auto grid = QVector<QVector<char>>(max.y() - min.y() + 1, QVector<char>(max.x() - min.x() + 1, '.'));

            // Add the origin
            grid[-min.y()][-min.x()] = 's';

            // Add the knots
            for(auto& knot: knots)
            {
                char tailName                                = (knots.size() == 1) ? 'T' : '1' + knots.indexOf(knot);
                grid[knot.y() - min.y()][knot.x() - min.x()] = tailName;
            }

            // Add the head
            grid[head.y() - min.y()][head.x() - min.x()] = 'H';

            // Print the grid in reverse
            for(auto row = grid.crbegin(); row != grid.crend(); row++)
            {
                // Print the row
                QString rowString;
                for(auto& cell: *row)
                {
                    rowString.append(cell);
                }
                qDebug().noquote() << rowString;
            }
        }

    private:
        QPoint        head{0, 0};

        QList<QPoint> knots;

        QList<QPoint> tailHistory;
};

int main(int argc, char* argv[]) {
    // Get input file
    QFile inputFile = QFile("input.txt");
    //QFile inputFile = QFile("testInput2.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
        return 1;
    }

    // Parse instructions
    QList<std::pair<Qt::ArrowType, int>> instructions;
    while(!inputFile.atEnd())
    {
        QString       instruction        = inputFile.readLine().trimmed();

        auto          split              = instruction.split(" ");
        int           distance           = split.last().toInt();
        char          directionSpecifier = split.first().toLatin1().at(0);

        Qt::ArrowType direction;
        switch(directionSpecifier)
        {
            case 'U':
                direction = Qt::UpArrow;
                break;
            case 'D':
                direction = Qt::DownArrow;
                break;
            case 'L':
                direction = Qt::LeftArrow;
                break;
            case 'R':
                direction = Qt::RightArrow;
                break;
            case Qt::NoArrow:
            default:
                qDebug() << "Elf: Call me Losto. Got direction:" << directionSpecifier;
                break;
        }

        // Append directions
        instructions.append(std::make_pair(direction, distance));
    }

    // Create a rope
    Rope rope(10);

    // Apply instructions
    for(const auto& instruction: instructions)
    {
        rope.move(instruction.first, instruction.second);
    }

    // Print results
    qDebug() << "Unique visits:" << rope.history().count();

    return 0;
}