#ifndef _RWVIEWER_WORLDVIEWER_HPP_
#define _RWVIEWER_WORLDVIEWER_HPP_
#include <engine/GameData.hpp>
#include <engine/GameWorld.hpp>

#include "ViewerInterface.hpp"

#include <QLabel>
#include <QLayout>
#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>

class WorldViewer : public ViewerInterface {
    Q_OBJECT

    QVBoxLayout* mainLayout;
    ViewerWidget* viewerWidget = nullptr;

public:
    WorldViewer(QWidget* parent = 0, Qt::WindowFlags f = 0);

signals:
    void placementsLoaded(const QString& file);

public slots:
    void loadPlacements(const QString& file);
    void loadPlacements();
};

#endif
