#include <QtCore>
#include <bitset>

// I've got 99 bits and memory problems are none!
std::bitset<99> parseAssignment(QStringView assignment) {
    auto assigmentIndicies = assignment.split('-');
    // Sanity check
    if(assigmentIndicies.count() != 2)
    {
        qDebug() << "Help: From the assignment:" << assignment << "I got" << assigmentIndicies.count() << "indicies";
    }

    // Set the bits
    std::bitset<99> bitset;
    size_t          start = assigmentIndicies.first().toInt();
    size_t          end   = assigmentIndicies.last().toInt();
    for(size_t i = start; i <= end; i++)
    {
        bitset.set(i - 1);
    }

    return bitset;
}

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    // Get input file
    QFile inputFile = QFile("input.txt");
    // QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
    }

    int overlapCount = 0;
    int partialOverlapCount = 0;
    while(!inputFile.atEnd())
    {
        QString assignmentLine = inputFile.readLine();

        // Remove the newline character
        assignmentLine.remove('\n');

        // Split the input
        auto split = assignmentLine.split(",");

        // Sanity check
        if(split.count() != 2)
        {
            qDebug() << "Elf: What kind of nasty split is this?";
        }

        // Assign both assignments
        auto assignment1 = parseAssignment(split.first());
        auto assignment2 = parseAssignment(split.last());

        // Check full overlap
        auto overlap = assignment1 & assignment2;
        if(overlap == assignment1 || overlap == assignment2)
        {
            overlapCount++;
        }

        // Check for partial overlap
        if(overlap.any())
        {
            partialOverlapCount++;
        }
    }
    
    // Print value
    qDebug() << "Overlap count:" << overlapCount;
    qDebug() << "Partial overlap count:" << partialOverlapCount;

    return a.exec();
}
