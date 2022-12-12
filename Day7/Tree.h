#pragma once
#include "TreeItems.h"
#include <QtCore>

class Tree {
    public:
        Tree(Folder* folder)
              : m_currentFolder(folder) { }

        bool cdUp() {
            // Check what's above
            auto upFolder = m_currentFolder->parent();

            // Check if we are no root
            if(upFolder == nullptr)
            {
                return false;
            }

            // Go up
            m_currentFolder = upFolder;
            return true;
        }

        bool cd(QString directory) {
            for(const auto item: m_currentFolder->contents())
            {
                if(Folder* folder = dynamic_cast<Folder*>(item); folder != nullptr)
                {
                    if(item->name() == directory)
                    {
                        m_currentFolder = folder;
                        return true;
                    }
                }
            }

            return false;
        }

        bool addItem(TreeItem* item) {
            // Check if we have a file with the same name
            for(const auto existingItem: m_currentFolder->contents())
            {
                if(existingItem->name() == item->name())
                {
                    return false;
                }
            }

            // Add the item
            m_currentFolder->addItem(item);
            return true;
        }

        void toRoot() {
            while(cdUp()) { }
        }

        // Print the file tree
        void print() const {
            qDebug() << "Printing file tree";
            qDebug().noquote() << m_currentFolder->info().join("\n");
        }

        QList<std::pair<TreeItem*, size_t>> sizeList() const {
            QList<std::pair<TreeItem*, size_t>> list;
            for(const auto& item: itemList())
            {
                list.append({item, item->size()});
            }

            // Sort on size
            std::sort(list.begin(), list.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
            return list;
        }

        QList<TreeItem*> itemList() const {
            // Start with the current folder
            QList<TreeItem*> list = {m_currentFolder};

            // Work down from there
            for(const auto item: m_currentFolder->contents())
            {
                // Append the folders
                if(Folder* folder = dynamic_cast<Folder*>(item); folder != nullptr)
                {
                    // Create a temporary copy to ensure constness
                    auto tempTree = Tree(folder);
                    
                    // Recursively add it's contents
                    list.append(tempTree.itemList());
                }
                else
                {
                    // Append the item
                    list.append(item);
                }
            }

            return list;
        }

        Folder* currentFolder() const {
            return m_currentFolder;
        }

    private:
        Folder* m_currentFolder = nullptr;
};
