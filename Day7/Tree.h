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

            return false;
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
            QList<std::pair<TreeItem*, size_t>> list;
            for(const auto& item: itemList())
            {
                list.append({item, item->size()});
            }

            // Sort on size
            std::sort(list.begin(), list.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
            return list;
        }

        QList<TreeItem*> itemList(){
            // Start with the current folder
            QList<TreeItem*> list = {currentFolder};

            // Work down from there
            for(const auto item: currentFolder->contents())
            {
                // Append the folders
                if(Folder* folder = dynamic_cast<Folder*>(item); folder != nullptr)
                {                   
                    // Recursively add it's contents
                    auto current = currentFolder;
                    cd(folder->name());
                    list.append(itemList());
                    cdUp();
                }
                else
                {
                    // Append the item
                    list.append(item);
                }
            }

            return list;
        }

    private:
        Folder* currentFolder = nullptr;
};
