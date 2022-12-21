#include <QtCore>

class CPU {
    public:
        void noop() {
            step();
        }

        void add(int x) {
            step();
            step();
            accumulator += x;
        }

        void step() {
            // Update strength
            if((PC - 20) % 40 == 0)
            {
                signalStrength += PC * accumulator;
            }

            // Increase PC
            PC++;
        }

        int getSignal() {
            return signalStrength;
        }

    private:
        int PC             = 1;
        int accumulator    = 1;
        int signalStrength = 0;
};

int main(int argc, char* argv[]) {
    // Get input file
    QFile inputFile = QFile("input.txt");
    //QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
        return 1;
    }

    // Init processor
    auto PC = CPU();

    // Parse instructions
    // Basically we're gonna make a cpu
    while(!inputFile.atEnd())
    {
        QString instruction = inputFile.readLine().trimmed();

        auto    split       = instruction.split(" ");

        // Must be noop
        if(split.count() == 1)
        {
            // Sanity check
            if(split.first() != "noop")
            {
                qDebug() << "Unsupported instruction, expected noop got:" << split.first();
                return 1;
            }

            // Execute instruction
            PC.noop();
        }
        // Must be addx
        else if(split.count() == 2)
        {
            // Sanity check
            bool ok;
            int  argument = split.last().toInt(&ok);
            if(split.first() != "addx" || !ok)
            {
                qDebug() << "Unsupported instruction, expected addx <number> got:" << split.first() << split.last();
                return 1;
            }

            // Execute instruction
            PC.add(argument);
        }
    }

    // Print results
    qDebug() << "Signal strength:" << PC.getSignal();

    return 0;
}