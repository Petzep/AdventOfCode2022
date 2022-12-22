#include <QtCore>

#define part2

class Monkey {
    public:
        Monkey(QStringList input) {
            if(input.count() != 6)
            {
                qDebug() << "Unparsable";
            }

            // Monkey id
            if(QString monkeyLine = input.at(0); monkeyLine.contains("Monkey"))
            {
                monkeyId = monkeyLine.remove(QRegularExpression("\\D")).toInt();
            }
            else
            {
                qDebug() << "Could not find monkey id in line" << monkeyLine;
            }

            // Starting items
            if(QString startingItemsLine = input.at(1); startingItemsLine.contains("Starting items:"))
            {
                startingItemsLine = startingItemsLine.remove(QRegularExpression("[^\\d,]"));
                for(auto item: startingItemsLine.split(","))
                {
                    items.append(item.toInt());
                }
            }
            else
            {
                qDebug() << "Could not find starting items in line" << startingItemsLine;
            }

            // Operation
            if(QString operationLine = input.at(2); operationLine.contains("Operation:"))
            {
                operationLine  = operationLine.remove("Operation: new = ");
                auto operators = QRegularExpression("[*\\-+]");
                auto arguments = operationLine.split(operators);

                // Operation
                QRegularExpressionMatch match         = operators.match(operationLine);
                auto                    operationSign = match.captured();

                // Functor magic
                operation = [this, operationSign, arguments](uint64_t item) -> uint64_t {
                    // Parse numbers

                    bool     aOk;
                    uint64_t a = arguments.first().toInt(&aOk);
                    if(!aOk && arguments.first().contains("old"))
                    {
                        a = item;
                    }

                    bool     bOk;
                    uint64_t b = arguments.last().toInt(&bOk);
                    if(!bOk && arguments.last().contains("old"))
                    {
                        b = item;
                    }


                    if(operationSign == "+")
                    {
                        return item = a + b;
                    }
                    else if(operationSign == "-")
                    {
                        return item = a - b;
                    }
                    else if(operationSign == "*")
                    {
                        return item = a * b;
                    }
                    else
                    {
                        qWarning() << "Error on operator";
                    }
                };
            }
            else
            {
                qDebug() << "Could not find operation in line" << operationLine;
            }

            // Test
            if(QString testLine = input.at(3); testLine.contains("Test:"))
            {
                divideNumber = testLine.remove(QRegularExpression("\\D")).toInt();
            }
            else
            {
                qDebug() << "Could not find test in line" << testLine;
            }

            // True condition
            if(QString trueLine = input.at(4); trueLine.contains("If true:"))
            {
                trueThrowDestination = trueLine.remove(QRegularExpression("\\D")).toInt();
            }
            else
            {
                qDebug() << "Could not find trueLine in line" << trueLine;
            }

            // False condition
            if(QString falseLine = input.at(5); falseLine.contains("If false:"))
            {
                falseThrowDestination = falseLine.remove(QRegularExpression("\\D")).toInt();
            }
            else
            {
                qDebug() << "Could not find falseLine in line" << falseLine;
            }
        }

        std::pair<uint64_t, int> doTask() {
            inspectedItems++;
            auto item = operation(items.takeFirst());
#ifndef part2
            item /= 3;
#endif
            return {item, (item % divideNumber) ? falseThrowDestination : trueThrowDestination};
        }

        QList<uint64_t> items;

        int             id() const { return monkeyId; }
        uint64_t        curiosity() const { return inspectedItems; }
        int             divisor() const { return divideNumber; }

    private:
        int                               monkeyId;
        uint64_t                          inspectedItems = 0;

        std::function<uint64_t(uint64_t)> operation;
        int                               divideNumber;
        int                               trueThrowDestination;
        int                               falseThrowDestination;
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

    // Create monkeys
    QList<Monkey*> monkeys;
    while(!inputFile.atEnd())
    {
        QStringList monkeyDescription;
        QString     inputLine = inputFile.readLine().trimmed();
        while(inputLine.count() > 0)
        {
            monkeyDescription.append(inputLine);
            inputLine = inputFile.readLine().trimmed();
        }

        monkeys.append(new Monkey(monkeyDescription));
    }

    // Run rounds
#ifdef part2
    int rounds = 10000;
#else
    int rounds = 20;
#endif

    // Find the stressreliever with the Chinese remainder theorem.
    uint64_t weedFactor = 1;
    for(const auto& monkey: monkeys)
    {
        weedFactor *= monkey->divisor();
    }

    for(int round = 0; round < rounds; round++)
    {
        for(auto& monkey: monkeys)
        {
            // Check if he has items
            while(!monkey->items.isEmpty())
            {
                // Inspect
                auto result = monkey->doTask();
#ifdef part2
                monkeys[result.second]->items.append(result.first % weedFactor);
#else
                monkeys[result.second]->items.append(result.first);
#endif
            };
        }
    }

    // Print results
    for(const auto& monkey: monkeys)
    {
        QStringList itemList;
        for(const auto& item: monkey->items)
        {
            itemList.append(QString::number(item));
        }
        qDebug().noquote() << QString("Monkey %1: %2").arg(monkey->id()).arg(itemList.join(","));
    }

    // Sort on curiosity
    std::ranges::sort(monkeys, [](Monkey* a, Monkey* b) { return a->curiosity() > b->curiosity(); });

    qDebug() << "\nCuriosity\n-------------------";
    for(const auto& monkey: monkeys)
    {
        qDebug().noquote() << QString("Monkey %1: %2").arg(monkey->id()).arg(monkey->curiosity());
    }

    uint64_t monkeyBuisness = monkeys.at(0)->curiosity() * monkeys.at(1)->curiosity();
    qDebug() << "\nMonkeyBuisness:" << monkeyBuisness;

    return 0;
}