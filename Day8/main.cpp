#include <QtCore>

int main(int argc, char* argv[]) {
    // Get input file
    QFile inputFile = QFile("input.txt");
    // QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
        return 1;
    }

    // Read the map
    QList<QList<int>> treeHeight;
    QList<const int*> visiableTrees;
    while(!inputFile.atEnd())
    {
        QString    line = inputFile.readLine().trimmed();
        QList<int> treeLine;
        for(const auto& c: line)
        {
            if(!c.isNumber())
            {
                qDebug() << "This must be a Christmas tree";
                return 1;
            }
            treeLine.append(c.toLatin1() - '0');
        }
        treeHeight.append(treeLine);
    }

    QList<int> maxLine(treeHeight.first().size(), -1);
    std::ranges::for_each(treeHeight, [&visiableTrees, &maxLine](const auto& treeLine) {
        int  maxHeight = -1;
        auto maxLineIt = maxLine.begin();

        for(const auto& tree: treeLine)
        {
            bool added = false;

            // Look at the trees from left to right
            if(tree > maxHeight)
            {
                maxHeight = tree;
                added     = true;
            }

            // Look at the trees from top to bottom
            if(tree > *maxLineIt)
            {
                *maxLineIt = tree;
                added      = true;
            }

            // Increment iterator
            maxLineIt++;

            // Add the tree
            if(added && !visiableTrees.contains(&tree))
            {
                visiableTrees.append(&tree);
            }
        }
    });

    // Now from the other side
    maxLine.fill(-1);
    for(auto treeLine = treeHeight.crbegin(); treeLine != treeHeight.crend(); treeLine++)
    {
        int  maxHeight = -1;
        auto maxLineIt = maxLine.rbegin();

        for(auto tree = (*treeLine).crbegin(); tree != (*treeLine).crend(); tree++)
        {
            bool added = false;

            // Look at the trees from left to right
            if(*tree > maxHeight)
            {
                maxHeight = *tree;
                added     = true;
            }

            // Look at the trees from top to bottom
            if(*tree > *maxLineIt)
            {
                *maxLineIt = *tree;
                added      = true;
            }

            // Increment iterator
            maxLineIt++;

            // Add the tree
            if(added && !visiableTrees.contains(tree.base() - 1))
            {
                visiableTrees.append(tree.base() - 1);
            }
        }
    };

    qDebug() << "Trees visible: " << visiableTrees.size();

    // Bah, this could have been more effective
    int maxScenicScore = 0;
    for(int y = 1; y < treeHeight.size() - 1; y++)
    {
        for(int x = 1; x < treeHeight[y].size() - 1; x++)
        {
            // Calculate the scenic score
            int score = 1;
            // Look left
            int lineScore  = 0;
            int lineHeight = 0;
            for(int i = x - 1; i >= 0; i--)
            {
                if(treeHeight[y][i] < lineHeight)
                {
                    // Ohhhhh, its about viewing distance
                    lineScore++;
                    continue;
                }
                else if(treeHeight[y][i] < treeHeight[y][x])
                {
                    lineScore++;
                    lineHeight = treeHeight[y][i];
                }
                else
                {
                    lineScore++;
                    break;
                }
            }

            // Update variables
            score *= lineScore;

            // Look right
            lineScore  = 0;
            lineHeight = 0;
            for(int i = x + 1; i < treeHeight[y].size(); i++)
            {
                lineScore++;
                if(treeHeight[y][i] < lineHeight)
                {
                    continue;
                }
                else if(treeHeight[y][i] < treeHeight[y][x])
                {
                    lineHeight = treeHeight[y][i];
                }
                else
                {
                    break;
                }
            }

            // Update variables
            score *= lineScore;

            // Look up
            lineScore  = 0;
            lineHeight = 0;
            for(int i = y - 1; i >= 0; i--)
            {
                lineScore++;
                if(treeHeight[i][x] < lineHeight)
                {
                    continue;
                }
                else if(treeHeight[i][x] < treeHeight[y][x])
                {
                    lineHeight = treeHeight[i][x];
                }
                else
                {
                    break;
                }
            }

            // Update variables
            score *= lineScore;

            // Look down
            lineScore  = 0;
            lineHeight = 0;
            for(int i = y + 1; i < treeHeight.size(); i++)
            {
                lineScore++;
                if(treeHeight[i][x] < lineHeight)
                {
                    continue;
                }
                else if(treeHeight[i][x] < treeHeight[y][x])
                {
                    lineHeight = treeHeight[i][x];
                }
                else
                {
                    break;
                }
            }

            // Update variables
            score *= lineScore;
            maxScenicScore = std::max(score, maxScenicScore);
        }
    }

    // Print results
    qDebug() << "Max Scenic Score: " << maxScenicScore;

    return 0;
}