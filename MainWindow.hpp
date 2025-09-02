#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QProcess> // Include QProcess header here

// Forward declarations
class QLineEdit;
class QPlainTextEdit;
class QCheckBox;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // UI Actions
    void onNewSync();
    void onOpenSync();
    void onEditSync();
    void onRenameSync();
    void onDeleteSync();
    void onAbout();

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

    // --- UI Elements ---
    QLineEdit *sourceEdit;
    QLineEdit *destinationEdit;
    QPlainTextEdit *outputView;

    // Options
    QCheckBox *archiveCheck;
    QCheckBox *verboseCheck;
    QCheckBox *progressCheck;
    QCheckBox *deleteCheck;
    QCheckBox *sizeOnlyCheck;
    QCheckBox *ignoreExistingCheck;
    QCheckBox *skipNewerCheck;

    // Buttons
    QPushButton *runButton;
    QPushButton *stopButton;


    // --- Process ---
    QProcess *rsyncProcess;
};

#endif // MAINWINDOW_HPP