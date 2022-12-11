#pragma once
#include <QtCore>

// Base item
class TreeItem {
        friend class Folder;

    public:
        TreeItem(QString name)
              : m_name(name){};

        QString name() const { return m_name; }

        Folder* parent() const {
            return m_parent;
        }

        virtual QStringList info() const = 0;

        virtual size_t  size() const = 0;

    protected:
        QString m_name;
        Folder* m_parent = nullptr;
};

// File item
class File : public TreeItem {
    public:
        File(QString name, size_t size)
              : TreeItem(name), m_size(size){};

        size_t  size() const override { return m_size; }

        QStringList info() const override {
            return {QString("- %1 (file, size=%2)").arg(m_name).arg(m_size)};
        }

    private:
        size_t m_size = 0;
};

// Folder item
class Folder : public TreeItem {
    public:
        Folder(QString name)
              : TreeItem(name){};

        bool addItem(TreeItem* item) {
            // Make sure it's not already added
            if(m_contents.contains(item))
            {
                return false;
            }

            // Set the parent and append;
            item->m_parent = this;
            m_contents.append(item);
            return true;
        }

        // Returns contents
        QList<TreeItem*> contents() const {
            return m_contents;
        }

        // Print info
        QStringList info() const override {
            QStringList contentList;
            contentList.append(QString("- %1 (dir)").arg(m_name));
            for(auto const& item : contents())
            {
                auto itemInfo = item->info().join("\n  ");
                contentList.append(QString("  %1").arg(itemInfo));
            }
            return contentList;
         }

        // Return size of items
        size_t size() const {
            // Book keeping
            size_t totalSize = 0;

            // Find the size of it's contents
            for(const auto& item: contents())
            {
                totalSize += item->size();
            }

            // Return size
            return totalSize;
        }

    private:
        QList<TreeItem*> m_contents;
};
