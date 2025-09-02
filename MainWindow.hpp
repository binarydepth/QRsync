#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QProcess>

// Forward declarations
class QLineEdit;
class QPlainTextEdit;
class QCheckBox;
class QPushButton;
class QJsonObject;
class QMenu;
class QAction;
class QActionGroup;
class QGroupBox;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // Menu Actions
    void onNew();
    void onLoad(const QString &name);
    void onSave(const QString &name);
    void onRename(const QString &name);
    void onDelete(const QString &name);
    void onAbout();
    void onModeContents();
    void onModeMirror();
    void onManualModeToggled(bool checked);
    void onArchiveToggled(bool checked);

    // UI Actions
    void onBrowseSource();
    void onBrowseDestination();
    void onRunSync();
    void onStopSync();

    // QProcess signals
    void onRsyncOutput();
    void onRsyncError();
    void onRsyncFinished(int exitCode, QProcess::ExitStatus exitStatus);


private:
    void setupUI();
    void setupMenuBar();
    void populateSyncsetMenus();
    void applySyncset(const QJsonObject &syncset);

    QJsonObject loadSyncsets();
    void saveSyncsets(const QJsonObject &syncsets);

    // --- UI Elements ---
    QLineEdit *sourceEdit;
    QLineEdit *destinationEdit;
    QPlainTextEdit *outputView;

    // Options
    QCheckBox *archiveCheck;
    QCheckBox *recursiveCheck;
    QCheckBox *permsCheck;
    QCheckBox *timesCheck;
    QCheckBox *groupCheck;
    QCheckBox *ownerCheck;
    QCheckBox *symlinksCheck;
    // ---
    QCheckBox *verboseCheck;
    QCheckBox *progressCheck;
    QCheckBox *deleteCheck;
    QCheckBox *sizeOnlyCheck;
    QCheckBox *ignoreExistingCheck;
    QCheckBox *skipNewerCheck;
    //---
    QLineEdit *manualOptionsEdit;

    // Buttons
    QPushButton *runButton;
    QPushButton *stopButton;

    // Menus & Actions
    QMenu *loadMenu;
    QMenu *saveMenu;
    QMenu *renameMenu;
    QMenu *deleteMenu;
    QActionGroup *modeActionGroup;
    QAction *contentsAction;
    QAction *mirrorAction;
    QAction *manualAction;


    // --- Process & Settings ---
    QProcess *rsyncProcess;
    QString settingsFilePath;
};

#endif // MAINWINDOW_HPP