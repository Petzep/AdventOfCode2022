#include <QtCore>

#define part2

class RPC {
    public:
        // I hate this kind of constructors, they serve to allow placeholders
        explicit RPC(){};

        // Dereferenced constructor <3
        RPC(QString stringValue)
              : RPC(stringValue[0]) {
            if(stringValue.length() > 1)
            {
                qDebug() << "Man, please use a char not a string";
            }
        }

        // Constructor
        RPC(QChar value) {
            QList<QChar> myValues{'X', 'Y', 'Z'};
            QList<QChar> theirValues{'A', 'B', 'C'};

            if(myValues.contains(value))
            {
                this->value = value.toLatin1() - myValues.first().toLatin1();
            }
            else if(theirValues.contains(value))
            {
                this->value = value.toLatin1() - theirValues.first().toLatin1();
            }
            else
            {
                this->value = 0;
                qDebug() << "Dude, this is not a value";
            }
        }

        // Explicit int constructor, prevent abuse
        explicit RPC(int value)
              : value(value){};

        RPC giveWin() {
            return RPC((value + 3 + 1) % 3);
        }

        RPC giveDraw() {
            return RPC(value);
        }

        RPC giveLose() {
            return RPC((value + 3 - 1) % 3);
        }

        // Easy casting
        operator int() const { return value; }

        // Compares "Better hand"
        friend bool operator<(const RPC& lhs, const RPC& rhs) {
            return (lhs.value + 3 + 1) % 3 == rhs.value;
        }

        // Compares "Worse hand"
        friend bool operator>(const RPC& lhs, const RPC& rhs) {
            return (lhs.value + 3 - 1) % 3 == rhs.value;
        }

        // Compares "Draw"
        friend bool operator==(const RPC& lhs, const RPC& rhs) {
            return lhs.value == rhs.value;
        }

    private:
        // Private value, so hidden
        int value;
};


int score(std::pair<RPC, RPC> set) {
    // Calculate win score
    int winScore;

    // Draw
    if(set.first == set.second)
    {
        winScore = 3;
    }
    // Win
    else if(set.first < set.second)
    {
        winScore = 6;
    }
    // Lose
    else if(set.first > set.second)
    {
        winScore = 0;
    }
    else
    {
        winScore = 0xcafe;
        qDebug() << "This is a Christmas miracle!";
    }

    return winScore + (set.second + 1);
}

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    // Get input file
    QFile inputFile = QFile("input.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
    }

    // Read input
    QList<std::pair<RPC, RPC>> stratGuide;
    while(!inputFile.atEnd())
    {
        // Split line
        QString line = inputFile.readLine();
        line.remove('\n');
        QStringList lineSplit = line.split(" ");

        // Sanity check
        if(lineSplit.count() != 2)
        {
            qDebug() << "AWWWMAAIIIGAWDDD, what's this kind of strategy!";
            continue;
        }

        // Check if they all one letter
        if(!std::ranges::all_of(lineSplit.constBegin(), lineSplit.constEnd(), [](QString shouldBeChar) { return shouldBeChar.length() == 1; }))
        {
            qDebug() << "Okay, I've messed up the chars";
            return 0;
        }

#ifdef part2
        // Determine needed hand
        auto hint = lineSplit.last();
        RPC  hisHand = lineSplit.first();
        RPC  myHand;
        switch(hint[0].toLatin1())
        {
            case 'X':
                myHand = hisHand.giveLose();
                break;
            case 'Y':
                myHand = hisHand.giveDraw();
                break;
            case 'Z':
                myHand = hisHand.giveWin();
                break;
            default:
                qDebug() << "This is wizardery, Harry, you there?";
                break;
        }

        stratGuide.append({hisHand, myHand});
#else
        // Add to strategy guide
        stratGuide.append({lineSplit.first(), lineSplit.last()});
#endif // part2
    }

    int totalScore = std::accumulate(stratGuide.constBegin(), stratGuide.constEnd(), 0, [](int sum, const auto current) { return sum + score(current); });

    qDebug() << "Final score:" << totalScore;

    /*
    // Tests for the non-believers ;-)
    bool shouldBeDraw   = RPC("A") == RPC("A");
    bool shouldBeDraw2  = RPC("A") == RPC("X");
    bool shouldBeTrue1  = RPC("A") < RPC("B");
    bool shouldBeTrue2  = RPC("B") < RPC("C");
    bool shouldBeTrue3  = RPC("C") < RPC("A");
    bool shouldBeTrueX  = RPC("B") <= RPC("Y");
    bool shouldBeFalse1 = RPC("X") > RPC("Y");
    bool shouldBeFalse2 = RPC("Y") > RPC("Z");
    bool shouldBeFalse3 = RPC("Z") > RPC("X");
    bool shouldBeFalseX = RPC("X") > RPC("X");
    */

    return a.exec();
}
