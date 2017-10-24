rwviewer_sources = files(
    'main.cpp',

    'ViewerWindow.cpp',

    'models/ObjectListModel.cpp',
    'models/DFFFramesTreeModel.cpp',

    'views/ObjectViewer.cpp',
    'views/ModelViewer.cpp',
    'views/WorldViewer.cpp',

    'widgets/ModelFramesWidget.cpp',

    'ViewerWidget.cpp',
    'ItemListModel.cpp',
    'ItemListWidget.cpp',
    'IMGArchiveModel.cpp',
    'AnimationListModel.cpp',
    'AnimationListWidget.cpp',
)

rwviewer_headers = files(
    'models/DFFFramesTreeModel.hpp',
    'models/ObjectListModel.hpp',

    'views/ViewerInterface.hpp',
    'views/ModelViewer.hpp',
    'views/ObjectViewer.hpp',
    'views/WorldViewer.hpp',

    'widgets/ModelFramesWidget.hpp',

    'AnimationListModel.hpp',
    'AnimationListWidget.hpp',
    'IMGArchiveModel.hpp',
    'ItemListModel.hpp',
    'ItemListWidget.hpp',
    'ViewerWidget.hpp',
    'ViewerWindow.hpp',
)

module_qt5 = import('qt5')
dep_qt5_rwviewer = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'OpenGL'], required: true)

rwviewer_moc_files = module_qt5.preprocess(moc_headers: rwviewer_headers)

rwviewer = executable('rwviewer', rwviewer_sources, rwviewer_moc_files, dependencies: [dep_bullet, dep_openal, dep_qt5_rwviewer], link_with: [rwengine], include_directories: [rwlib_include, rwengine_include])
