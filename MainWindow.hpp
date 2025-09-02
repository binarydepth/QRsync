#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QProcess> // Include QProcess header here

// Forward declarations
class QLineEdit;
class QPlainTextEdit;

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

    // --- Process ---
    QProcess *rsyncProcess;
};

#endif // MAINWINDOW_HPP