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

            pulse();

            // Increase PC
            PC++;

            // Increase beam
            beam++;
            if(beam == screen.end())
            {
                beam = screen.begin();
            }
        }

        void pulse() {
            int beamPosition = std::distance(screen.begin(), beam) % screenW;
            int spriteOfset  = (spritLength / 2);
            if(abs(accumulator - beamPosition) <= spriteOfset)
            {
                *beam = true;
            }
            else
            {
                *beam = false;
            }
        }

        int getSignal() {
            return signalStrength;
        }

        void printScreen() {
            for(int y = 0; y < screenH; y++)
            {
                QString scanline;
                for(int x = 0; x < screenW; x++)
                {
                    if(screen[y * screenW + x])
                    {
                        scanline.append("#");
                    }
                    else
                    {
                        scanline.append(".");
                    }
                }
                qDebug().noquote() << scanline;
            }
        }

    private:
        int                   PC             = 1;
        int                   accumulator    = 1;
        int                   signalStrength = 0;
        const int             spritLength    = 3;

        const int             screenW        = 40;
        const int             screenH        = 6;

        QList<bool>           screen         = QList<bool>(screenH * screenW, false);
        QList<bool>::iterator beam           = screen.begin();
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

    PC.printScreen();

    return 0;
}