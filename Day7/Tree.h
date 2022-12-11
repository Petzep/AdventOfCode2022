#pragma once
#include "TreeItems.h"
#include <QtCore>

class Tree {
    public:
        Tree(Folder* folder)
              : currentFolder(folder) { }

        bool cdUp() {
            // Check what's above
            auto upFolder = currentFolder->parent();

            // Check if we are no root
            if(upFolder == nullptr)
            {
                return false;
            }

            // Go up
            currentFolder = upFolder;
            return true;
        }

        bool cd(QString directory) {
            for(const auto item: currentFolder->contents())
            {
                if(Folder* folder = dynamic_cast<Folder*>(item); folder != nullptr)
                {
                    if(item->name() == directory)
                    {
                        currentFolder = folder;
                        return true;
                    }
                }
            }
        }

        bool addItem(TreeItem* item) {
            // Check if we have a file with the same name
            for(const auto existingItem: currentFolder->contents())
            {
                if(existingItem->name() == item->name())
                {
                    return false;
                }
            }

            // Add the item
            currentFolder->addItem(item);
            return true;
        }

        void toRoot() {
            while(cdUp()) { }
        }

        // Print the file tree
        void print() {
            qDebug() << "Printing file tree";
            qDebug().noquote() << currentFolder->info().join("\n");
        }

        QList<std::pair<TreeItem*, size_t>> sizeList() {
            QList<std::pair<TreeItem*, size_t>> sizeList;
            for(const auto item: currentFolder->contents())
            {
                sizeList.append({item, item->size()});
            }
            return sizeList;
        }

    private:
        Folder* currentFolder = nullptr;
};
