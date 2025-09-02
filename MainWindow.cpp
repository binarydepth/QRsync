#include "MainWindow.hpp"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), rsyncProcess(nullptr) {
    setupUI();
    rsyncProcess = new QProcess(this);

    connect(rsyncProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onRsyncOutput);
    connect(rsyncProcess, &QProcess::readyReadStandardError, this, &MainWindow::onRsyncError);
    connect(rsyncProcess, &QProcess::finished, this, &MainWindow::onRsyncFinished);

    // Initial button state
    stopButton->setEnabled(false);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("QRsync");
    resize(800, 600);

    setupMenuBar();

    // --- Central Widget and Layout ---
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // --- Paths Group ---
    QGroupBox *pathsGroup = new QGroupBox("Paths");
    QGridLayout *pathsLayout = new QGridLayout(pathsGroup);

    pathsLayout->addWidget(new QLabel("Source:"), 0, 0);
    sourceEdit = new QLineEdit();
    pathsLayout->addWidget(sourceEdit, 0, 1);
    QPushButton *browseSourceButton = new QPushButton("Browse...");
    connect(browseSourceButton, &QPushButton::clicked, this, &MainWindow::onBrowseSource);
    pathsLayout->addWidget(browseSourceButton, 0, 2);

    pathsLayout->addWidget(new QLabel("Destination:"), 1, 0);
    destinationEdit = new QLineEdit();
    pathsLayout->addWidget(destinationEdit, 1, 1);
    QPushButton *browseDestinationButton = new QPushButton("Browse...");
    connect(browseDestinationButton, &QPushButton::clicked, this, &MainWindow::onBrowseDestination);
    pathsLayout->addWidget(browseDestinationButton, 1, 2);
    mainLayout->addWidget(pathsGroup);


    // --- Options Group ---
    QGroupBox *optionsGroup = new QGroupBox("Options");
    QGridLayout *optionsLayout = new QGridLayout(optionsGroup);

    archiveCheck = new QCheckBox("Archive (-a)");
    verboseCheck = new QCheckBox("Verbose (-v)");
    verboseCheck->setChecked(true); // Default
    progressCheck = new QCheckBox("Progress (--progress)");
    progressCheck->setChecked(true); // Default
    deleteCheck = new QCheckBox("Delete on destination (--delete)");
    sizeOnlyCheck = new QCheckBox("Size only (--size-only)");
    ignoreExistingCheck = new QCheckBox("Ignore existing (--ignore-existing)");
    skipNewerCheck = new QCheckBox("Skip newer (--update)");

    optionsLayout->addWidget(archiveCheck, 0, 0);
    optionsLayout->addWidget(verboseCheck, 0, 1);
    optionsLayout->addWidget(progressCheck, 0, 2);
    optionsLayout->addWidget(skipNewerCheck, 0, 3);
    optionsLayout->addWidget(deleteCheck, 1, 0);
    optionsLayout->addWidget(sizeOnlyCheck, 1, 1);
    optionsLayout->addWidget(ignoreExistingCheck, 1, 2);
    mainLayout->addWidget(optionsGroup);

    // --- Output Group ---
    QGroupBox *outputGroup = new QGroupBox("Output");
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    outputView = new QPlainTextEdit();
    outputView->setReadOnly(true);
    outputView->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    outputLayout->addWidget(outputView);
    mainLayout->addWidget(outputGroup);

    // --- Execution Buttons ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    runButton = new QPushButton("Run Sync");
    connect(runButton, &QPushButton::clicked, this, &MainWindow::onRunSync);

    stopButton = new QPushButton("Stop");
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopSync);

    buttonLayout->addStretch();
    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(stopButton);
    mainLayout->addLayout(buttonLayout);
}

void MainWindow::setupMenuBar() {
    QMenu *appMenu = menuBar()->addMenu("&QRsync");
    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, &QApplication::quit);
    appMenu->addAction(quitAction);

    QMenu *syncsetMenu = menuBar()->addMenu("&Syncsets");
    syncsetMenu->addAction("New Syncset", this, &MainWindow::onNewSync);
    syncsetMenu->addAction("Open Syncset", this, &MainWindow::onOpenSync);
    syncsetMenu->addSeparator();
    syncsetMenu->addAction("Edit Syncset", this, &MainWindow::onEditSync);
    syncsetMenu->addAction("Rename Syncset", this, &MainWindow::onRenameSync);
    syncsetMenu->addAction("Delete Syncset", this, &MainWindow::onDeleteSync);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("About QRsync", this, &MainWindow::onAbout);
}


// --- Slots Implementation ---

void MainWindow::onBrowseSource() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Source Directory");
    if (!directory.isEmpty()) {
        sourceEdit->setText(directory + "/");
    }
}

void MainWindow::onBrowseDestination() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Destination Directory");
    if (!directory.isEmpty()) {
        destinationEdit->setText(directory);
    }
}

void MainWindow::onRunSync() {
    QString source = sourceEdit->text();
    QString destination = destinationEdit->text();

    if (source.isEmpty() || destination.isEmpty()) {
        QMessageBox::warning(this, "Missing Paths", "Source and Destination paths cannot be empty.");
        return;
    }

    runButton->setEnabled(false);
    stopButton->setEnabled(true);
    outputView->clear();

    QStringList arguments;
    if (archiveCheck->isChecked()) arguments << "-a";
    if (verboseCheck->isChecked()) arguments << "-v";
    if (progressCheck->isChecked()) arguments << "--progress";
    if (deleteCheck->isChecked()) arguments << "--delete";
    if (sizeOnlyCheck->isChecked()) arguments << "--size-only";
    if (ignoreExistingCheck->isChecked()) arguments << "--ignore-existing";
    if (skipNewerCheck->isChecked()) arguments << "--update";

    arguments << source << destination;

    outputView->appendPlainText("--- Starting rsync ---");
    outputView->appendPlainText("rsync " + arguments.join(" "));
    outputView->appendPlainText("\n");

    rsyncProcess->start("rsync", arguments);
}

void MainWindow::onStopSync() {
    if (rsyncProcess->state() == QProcess::Running) {
        rsyncProcess->kill();
        outputView->appendPlainText("\n--- Process terminated by user. ---");
    }
}

// Syncset slots are placeholders for now
void MainWindow::onNewSync() {
    QMessageBox::information(this, "Not Implemented", "This feature will be implemented soon.");
}
void MainWindow::onOpenSync() {
    QMessageBox::information(this, "Not Implemented", "This feature will be implemented soon.");
}
void MainWindow::onEditSync() {
    QMessageBox::information(this, "Not Implemented", "This feature will be implemented soon.");
}
void MainWindow::onRenameSync() {
    QMessageBox::information(this, "Not Implemented", "This feature will be implemented soon.");
}
void MainWindow::onDeleteSync() {
    QMessageBox::information(this, "Not Implemented", "This feature will be implemented soon.");
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About QRsync",
                       "<h3>QRsync</h3>"
                       "<p>A simple Qt-based GUI for the rsync command-line tool.</p>"
                       "<p>Version 0.1</p>");
}


void MainWindow::onRsyncOutput() {
    QByteArray data = rsyncProcess->readAllStandardOutput();
    outputView->appendPlainText(QString::fromLocal8Bit(data));
}

void MainWindow::onRsyncError() {
    QByteArray data = rsyncProcess->readAllStandardError();
    outputView->appendPlainText(QString::fromLocal8Bit(data));
}

void MainWindow::onRsyncFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    QString status = (exitStatus == QProcess::NormalExit && exitCode == 0) ? "Success" : "Failed";
    outputView->appendPlainText(QString("\n--- Process finished with exit code %1 (%2) ---").arg(exitCode).arg(status));

    runButton->setEnabled(true);
    stopButton->setEnabled(false);
}