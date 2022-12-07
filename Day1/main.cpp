#include <QtCore>

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    // Get input file
    QFile inputFile = QFile("input.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
    }

    // Keep track of max
    int maxBag = 0;

    // Read input
    QList<QList<int>> calorieList;
    QList<int> calorieSum;
    while(!inputFile.atEnd())
    {
        // Create bag for elf
        QList<int> elfBag = QList<int>();

        // Read contents
        QString inputLine = inputFile.readLine();
        while(inputLine != "\n")
        {
            elfBag.append(inputLine.toInt());
            inputLine = inputFile.readLine();
        }

        // Add contents to list
        calorieList.append(elfBag);

        // Keep track of max
        int sum = std::accumulate(elfBag.constBegin(), elfBag.constEnd(), 0);
        calorieSum.append(sum);
        maxBag = qMax(maxBag, sum);
    }

    // Print bags
    qDebug() << "Bags:";
    qDebug() << "------------------";
    for(const auto& bag : calorieList)
    {
        for(const auto& calories : bag)
        {
            qDebug() << "- " << calories;
        }
        qDebug() << "-------------+";
        qDebug() << "Sum:" << std::accumulate(bag.constBegin(), bag.constEnd(), 0) << "\n";
    }

    // Print max
    qDebug() << "------------------";
    qDebug() << "Max bag is: " << maxBag;

    // Print top 3
    std::ranges::sort(calorieSum, std::ranges::greater());

    qDebug() << "\n-------- Top -------";
    int topSum = 0;
    for(int i = 0; i < 3; i++)
    {
        qDebug() << i << "  " << calorieSum[i];
        topSum += calorieSum[i];
    }
    
    // Print sum of top 3
    qDebug() << "------------------";
    qDebug() << "Top sum:" << topSum;

    return a.exec();
}
