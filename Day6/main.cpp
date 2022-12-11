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
    while(!inputFile.atEnd())
    {
        QQueue<QChar> startSequenceBuffer;
        int position = 0;
        int startOfPacket = 0;
        int startOfMessage = 0;
        QString line = inputFile.readLine();

        // Read character by character
        int distinceCharacters = 4;
        for(auto const& character : line)
        {
            // Fill buffer
            if(startSequenceBuffer.size() < distinceCharacters)
            {
                startSequenceBuffer.enqueue(character);
            }
            else
            {
                startSequenceBuffer.dequeue();
                startSequenceBuffer.enqueue(character);
            }

            // Check for duplicates in buffer
            if(startSequenceBuffer.size() == distinceCharacters)
            {
                bool duplicateFound = false;
                for(int i = 0; i < startSequenceBuffer.size(); i++)
                {
                    for(int j = i + 1; j < startSequenceBuffer.size(); j++)
                    {
                        if(startSequenceBuffer[i] == startSequenceBuffer[j])
                        {
                            duplicateFound = true;
                            break;
                        }
                    }
                    if(duplicateFound)
                        break;
                }

                if(!duplicateFound)
                {
                    if(distinceCharacters != 14)
                    {
                        startOfPacket = position + 1;
                        distinceCharacters = 14;
                    }
                    else
                    {
                        startOfMessage = position + 1;
                        break;
                    }
                }
            }
            position++;
        }

        // Print result
        qDebug() << "Start of packet: " << startOfPacket;
        qDebug() << "Start of message: " << startOfMessage;
        qDebug() << "Start sequence: " << startSequenceBuffer;
    }

    return 0;
}
