#include "Tree.h"
#include <QtCore>

int main(int argc, char* argv[]) {
    // Get input file
    QFile inputFile = QFile("input.txt");
    //QFile inputFile = QFile("testInput.txt");
    if(!inputFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        qDebug() << "Could not open file";
        return 1;
    }

    // Create the root folder
    Folder* root = new Folder("/");
    Tree*   tree = new Tree(root);

    // Read the terminal
    while(!inputFile.atEnd())
    {
        QString line  = inputFile.readLine();

        auto    parts = line.split(" ");

        // Parse commands
        if(parts.first() == "$")
        {
            const auto command = parts[1].trimmed();
            if(command == "cd")
            {
                // Sanity check
                if(parts.count() != 3)
                {
                    qDebug() << "This command is not right....it's a trap!!! (got command: " << line << ")";
                    break;
                }

                const auto folder = parts[2].trimmed();

                // We've already have a root
                if(folder == "/")
                {
                    continue;
                }
                else if(folder == "..")
                {
                    if(!tree->cdUp())
                    {
                        qDebug() << "We are already at the root";
                        break;
                    }
                }
                // Go to the folder
                else if(!tree->cd(folder))
                {
                    qDebug() << "Could not cd to " << folder;
                    break;
                }
            }
            else if(command == "ls")
            {
                // Parse the ls
                // Sanity check
                if(parts.count() != 2)
                {
                    qDebug() << "This command is not right....it's a trap!!! (got command: " << line << ")";
                    break;
                }
                continue;
            }
            else
            {
                qDebug() << "Got unknown command:" << command;
                break;
            }
        }
        // Parse output
        else
        {
            // We only have ls as output so we might as well parse it
            if(parts.count() != 2)
            {
                qDebug() << "This is no ls, we are being hacked! (got command: " << line << ")";
                break;
            }

            // Create a dir if needed
            QString name = parts.last().trimmed();

            if(parts.first() == "dir")
            {
                auto newFolder = new Folder(name);
                if(!tree->addItem(newFolder))
                {
                    qDebug() << "Could not add folder [" << name << "], is it already added?";
                    break;
                }
            }
            // We have a file
            else
            {
                auto newFile = new File(name, parts.first().toInt());
                if(!tree->addItem(newFile))
                {
                    qDebug() << "Could not add file [" << name << "], is it already added?";
                    break;
                }
            }
        }
    }

    // Print result
    tree->toRoot();
    tree->print();

    // Get item list
    auto itemList = tree->itemList();

    // Print the directories with a size of at most 100000
    size_t limit  = 100000;
    size_t topSum = 0;
    qDebug() << "\nFolders with at most a size of" << limit;
    for(const auto& item: itemList)
    {
        // Only list folders
        if(Folder* folder = dynamic_cast<Folder*>(item); folder != nullptr && folder->size() <= limit)
        {
            topSum += folder->size();

            qDebug() << folder->name() << "with size" << folder->size();
        }
    }

    qDebug() << "-----------------\nSum size:" << topSum;

    // Find the folder we need to delete
    size_t totalDiskSpace = 70000000;
    size_t freeSpaceNeeded = 30000000;
    
    tree->toRoot();
    size_t freeSpace = totalDiskSpace - tree->currentFolder()->size();
    size_t deletionSize = freeSpaceNeeded - freeSpace;
    
    // Print info
    qDebug() << "\n\nDisk information:\n----------------";
    qDebug() << "Total disk space:" << totalDiskSpace;
    qDebug() << "Free space:" << freeSpace;
    qDebug() << "We need to delete a folder with at least" << deletionSize << "free space";

    // Find the folder we need to delete
    QList<Folder*> deletionCandidates;
    for(const auto& item: itemList)
    {
        // Only list folders
        if(Folder* folder = dynamic_cast<Folder*>(item); folder != nullptr && folder->size() >= deletionSize)
        {
            deletionCandidates.append(folder);
        }
    }

    // Find the smallest folder that would work
    std::sort(deletionCandidates.begin(), deletionCandidates.end(), [](Folder* a, Folder* b) { return a->size() < b->size(); });
    qDebug() << "We need to delete:" << deletionCandidates.first()->name() << "with size:" << deletionCandidates.first()->size();
    
    
    return 0;
}