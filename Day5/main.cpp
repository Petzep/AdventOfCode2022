#include <QtCore>

#define part_2

int main(int argc, char* argv[]) {
    // Get input file
    QFile inputFile = QFile("input.txt");
    //QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
        return 1;
    }

    // Read the drawing
    QList<QString> drawingLines;
    while(!inputFile.atEnd())
    {
        QString line = inputFile.readLine();
        if(line == "\n")
        {
            break;
        }

        drawingLines.append(line.remove('\n'));
    }

    // Read the instructions
    QList<QString> instructionLines;
    while(!inputFile.atEnd())
    {
        instructionLines.append(inputFile.readLine());
    }

    // Close file
    inputFile.close();

    // Parse the drawing
    // Get the number of stacks
    auto stackLine = drawingLines.takeLast();
    int numberOfStacks = stackLine.split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts).last().toInt();

    // Create the stacks
    QList<QList<QChar>> stacks(numberOfStacks);

    for(auto& line : drawingLines)
    {
        // Unification of the input
        line.leftJustified(stackLine.length()); // Not needed, but just to be sure
        line.replace(QRegularExpression("(...)(\\s|$)"), "\\1|\\2");

        // Splitting of the stacks
        auto stackSplit = line.split('|', Qt::SplitBehaviorFlags::SkipEmptyParts);

        // Sanity check
        if(stackSplit.count() != numberOfStacks)
        {
            qWarning() << "Elf: I've got a new iPhone, it fell from a truck #CratesWithoutStack";
            return 1;
        }

        // Parse the stacks
        for(int i = 0; i < stackSplit.count(); i++)
        {
            auto stack = stackSplit[i];
            stack.remove(QRegularExpression("[\\[\\]]"));
            stack = stack.trimmed();
            if(stack.size() == 0)
            {
                continue;
            }
            else if(stack.count() != 1)
            {
                qWarning() << "We've got some trash letters left: [" << stack << "]";
                return 1;
            }

            // Place the stacks
            QChar id = stack.front();
            stacks[i].append(id);
        }
    }

    // Execute the instructions
    for(auto& instruction : instructionLines)
    {
        QRegularExpression re("\\d+");
        auto matchIterator = re.globalMatch(instruction);
        
        // Extract the amount
        auto match = matchIterator.next();
        if(!match.hasMatch())
        {
            qWarning() << "No match for the amount";
            return 1;
        }
        size_t amount = match.captured().toInt();
        
        // Extract the startStack
        match = matchIterator.next();
        if(!match.hasMatch())
        {
            qWarning() << "No match for the startStack";
            return 1;
        }
        size_t startStack = match.captured().toInt();

        // Extract the endStack
        match = matchIterator.next();
        if(!match.hasMatch())
        {
            qWarning() << "No match for the endStack";
            return 1;
        }
        size_t endStack = match.captured().toInt();
    
#ifdef part_2
        // Move the stacks as a whole
        QList<QChar> tempStack;
        for(int i = 0; i < amount; i++)
        {
            QChar topElement = stacks[startStack - 1].takeFirst();

            // Place the top element
            tempStack.prepend(topElement);
        }

        // Add tempstack to stack
        for(int i = 0; i < amount; i++)
        {
            stacks[endStack - 1].prepend(tempStack[i]);
        }
#else
        // Move the stacks
        for(int i = 0; i < amount; i++)
        {
            // Get the top element
            QChar topElement = stacks[startStack - 1].takeFirst();

            // Place the top element
            stacks[endStack - 1].prepend(topElement);
        }
#endif // part_2
    }

    // Take the first of each stack
    QString result;
    for(auto& stack : stacks)
    {
        result.append(stack.first());
    }
    
    // Print result
    qDebug() << "Result: " << result;
    return 0;
}