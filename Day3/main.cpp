#include <QtCore>

int letterValue(QChar letter) {
    int value = letter.toLatin1();

    if(letter.isUpper())
    {
        value += 27 - QChar('A').toLatin1();
    }
    else if(letter.isLower())
    {
        value += 1 - QChar('a').toLatin1();
    }
    else
    {
        qDebug() << "What are these hieroglyphs? I mean:" << letter << "How???";
    }

    return value;
}

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    // Get input file
    QFile inputFile = QFile("input.txt");
    //QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
    }

    QMap<QChar, int> letterCounts;
    QList<QChar>     groupList;
    QList<QChar>     uniqueLetters;
    int              groupCounter = 0;
    while(!inputFile.atEnd())
    {
        QString rucksack = inputFile.readLine();

        // Remove the newline character
        rucksack.remove('\n');

        // Split the input in half
        auto rucksack1 = rucksack.mid(0, rucksack.size() / 2);
        auto rucksack2 = rucksack.mid(rucksack.size() / 2);

        // Find the letters that appears in both rucksacks
        QChar commonLetter;
        for(int i = 0; i < rucksack1.length(); i++)
        {
            if(rucksack2.contains(rucksack1[i]))
            {
                letterCounts[rucksack1[i]]++;
                break;
            }
        }

        // Add the unique numbers
        if(groupCounter == 0)
        {
            for(int i = 0; i < rucksack.size(); i++)
            {
                auto letter = rucksack.at(i);
                if(!uniqueLetters.contains(letter))
                {
                    uniqueLetters.append(letter);
                }
            }
        }
        else
        {
            // Remove letters we don't have
            QList<QChar> finalUniqueLetters;
            for(int i = 0; i < rucksack.size(); i++)
            {
                auto letter = rucksack.at(i);
                if(uniqueLetters.contains(letter) && !finalUniqueLetters.contains(letter))
                {
                    finalUniqueLetters.append(letter);
                }
            }
            uniqueLetters.swap(finalUniqueLetters);
        }

        // Reset and check
        if(++groupCounter == 3)
        {
            if(uniqueLetters.size() != 1)
            {
                qDebug() << "SECURITY BREACH!!!!";
            }
            else
            {
                groupList.append(uniqueLetters.first());
            }

            groupCounter = 0;
            uniqueLetters.clear();
        }
    }

    // Calculate letter sum
    int letterSum = 0;
    for(QMap<QChar, int>::const_iterator it = letterCounts.constBegin(); it != letterCounts.constEnd(); it++)
    {
        letterSum += letterValue(it.key()) * it.value();
    };

    // Calculate the security group sum
    int groupSum = 0;
    for(const auto& letter: groupList)
    {
        groupSum += letterValue(letter);
    }

    // Print value
    qDebug() << "The sum of the letters is:" << letterSum;
    qDebug() << "The sum of the groups is:" << groupSum;


    return a.exec();
}
