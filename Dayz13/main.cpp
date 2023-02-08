#include <QDebug>
#include <QPoint>
#include <QtCore>
#include <QtMath>

QString packetToString(QVariant packet) {
    QString output;
    if(packet.type() == QVariant::List)
    {
        output += "[";
        for(int i = 0; i < packet.toList().size(); i++)
        {
            if(packet.toList()[i].type() == QVariant::List)
            {
                output += packetToString(packet.toList()[i]);
            }
            else
            {
                output += packet.toList()[i].toString();
            }

            if(i < packet.toList().size() - 1)
            {
                output += ",";
            }
        }
        output += "]";
    }
    if(packet.type() == QVariant::Int)
    {
        output += packet.toString();
    }

    return output;
}


QVariantList extractPayload(QString input) {
    QVariantList packet;

    // Extract payload recursively
    int first = -1;
    int index = -1;
    for(const auto& character: input)
    {
        index++;

        // This is an array, so search for the end
        if(first >= 0)
        {
            if(character == ']')
            {
                if(index - first == 1)
                {
                    // Add empty package
                    packet.append(QVariant());
                    packet[packet.size() - 1] = QVariantList();
                }
                else
                {
                    // Extract package line
                    auto packetLine = input.mid(first + 1, index - first - 1);
                    packet.append(QVariant());
                    packet[packet.size() - 1] = extractPayload(packetLine);
                }

                // Reset tracking
                first = -1;
            }
        }
        // Treat it normally
        else
        {
            // This is the end
            if(character == '[')
            {
                first = index;
                continue;
            }
            // Normal character
            else if(character == ',')
            {
                continue;
            }
            else
            {
                if(character.isNumber())
                {
                    packet.append(QVariant::fromValue<int>(character.toLatin1() - '0'));
                }
                else if(character != ']')
                {
                    qDebug() << "Invalid character: " << character;
                }
            }
        }
    }

    return packet;
}

int checkPacket(QVariant packA, QVariant packB) {
    auto test1 = packetToString(packA);
    auto test2 = packetToString(packB);

    // If both values are integers
    if(packA.type() == QVariant::Int && packB.type() == QVariant::Int)
    {
        // The A should be smaller or equal
        if(packA.toInt() < packB.toInt())
        {
            // Correct
            return 1;
        }
        else if(packA.toInt() > packB.toInt())
        {
            // Incorrect
            return -1;
        }
        else
        {
            // Indecisive
            return 0;
        }
    }

    // If both values are list, compare them element wise
    if(packA.type() == QVariant::List && packB.type() == QVariant::List)
    {
        // Check if they are equal
        auto listA = packA.toList();
        auto listB = packB.toList();

        // Compare each element
        auto minSize = qMin(listA.size(), listB.size());
        for(int i = 0; i < minSize; i++)
        {
            int result = checkPacket(listA[i], listB[i]);
            if(result != 0)
            {
                return result;
            }

            // Indecisive, continue
            continue;
        }

        // If the A is shorter, it's good
        return listA.size() <= listB.size();
    }

    // If exactly one of them is an integer
    if(packA.type() == QVariant::Int || packB.type() == QVariant::Int)
    {
        // Convert item to package with only that integer
        bool         aOk, bOk;
        QVariantList listA = QVariantList{packA.toInt(&aOk)};
        if(!aOk)
        {
            listA = QVariantList{packA.toList()};
        }

        QVariantList listB = QVariantList{packB.toInt(&bOk)};
        if(!bOk)
        {
            listB = QVariantList{packB.toList()};
        }

        return checkPacket(listA, listB);
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
    QVariantList packetList;
    while(!inputFile.atEnd())
    {
        QString line = inputFile.readLine().trimmed();
        if(line == "")
        {
            // TODO: take first pair, second pair
            continue;
        }

        //////////////////
        // Parse packet //
        //////////////////
        // Start with the most outer array
        auto first = line.indexOf('[');
        auto last  = line.lastIndexOf(']');

        // Detect singular empty package
        if(last - first == 1)
        {
            // Add empty package
            packetList.append(QVariant());
            packetList[packetList.size() - 1] = QVariantList();
        }
        else
        {
            // Extract package line
            auto packetLine = line.mid(first + 1, last - first - 1);
            packetList.append(QVariant());
            packetList[packetList.size() - 1] = extractPayload(packetLine);
        }
    }

    // Check and print packages
    for(int i = 0; i < packetList.size(); i += 2)
    {
        bool    correct       = checkPacket(packetList[i], packetList[i + 1]) == 1;
        QString packetString1 = packetToString(packetList[i]);
        QString packetString2 = packetToString(packetList[i + 1]);
        qDebug().noquote() << QString("Packet: %1\nPacket: %2\nCorrect: %3\n").arg(packetString1).arg(packetString2).arg(correct ? "Yes" : "No");
    }

    return 0;
}