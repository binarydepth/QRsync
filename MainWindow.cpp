#include "MainWindow.hpp"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), rsyncProcess(nullptr) {
    setupUI();
    rsyncProcess = new QProcess(this);

    connect(rsyncProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onRsyncOutput);
    connect(rsyncProcess, &QProcess::readyReadStandardError, this, &MainWindow::onRsyncError);
    connect(rsyncProcess, &QProcess::finished, this, &MainWindow::onRsyncFinished);
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
    QHBoxLayout *optionsLayout = new QHBoxLayout(optionsGroup);
    optionsLayout->addWidget(new QCheckBox("Archive (-a)"));
    optionsLayout->addWidget(new QCheckBox("Verbose (-v)"));
    optionsLayout->addWidget(new QCheckBox("Progress (--progress)"));
    optionsLayout->addWidget(new QCheckBox("Delete (--delete)"));
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
    QPushButton *runButton = new QPushButton("Run Sync");
    connect(runButton, &QPushButton::clicked, this, &MainWindow::onRunSync);

    QPushButton *stopButton = new QPushButton("Stop");
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopSync);

    buttonLayout->addStretch();
    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(stopButton);
    mainLayout->addLayout(buttonLayout);
}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, &QApplication::quit);
    fileMenu->addAction(quitAction);

    QMenu *profileMenu = menuBar()->addMenu("&Profiles");
    profileMenu->addAction("New Sync", this, &MainWindow::onNewSync);
    profileMenu->addAction("Open Sync", this, &MainWindow::onOpenSync);
    profileMenu->addSeparator();
    profileMenu->addAction("Edit Sync", this, &MainWindow::onEditSync);
    profileMenu->addAction("Rename Sync", this, &MainWindow::onRenameSync);
    profileMenu->addAction("Delete Sync", this, &MainWindow::onDeleteSync);
}


// --- Slots Implementation ---

void MainWindow::onBrowseSource() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Source Directory");
    if (!directory.isEmpty()) {
        sourceEdit->setText(directory);
    }
}

void MainWindow::onBrowseDestination() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Destination Directory");
    if (!directory.isEmpty()) {
        destinationEdit->setText(directory);
    }
}

void MainWindow::onRunSync() {
    // For now, this is a placeholder. We will implement this next.
    outputView->appendPlainText("--- Starting Sync (Placeholder) ---");
    outputView->appendPlainText("Source: " + sourceEdit->text());
    outputView->appendPlainText("Destination: " + destinationEdit->text());
    outputView->appendPlainText("--- Sync Finished ---");
}

void MainWindow::onStopSync() {
     outputView->appendPlainText("--- Stop Clicked (Placeholder) ---");
}

// Profile slots are placeholders for now
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


void MainWindow::onRsyncOutput() {
    outputView->appendPlainText(rsyncProcess->readAllStandardOutput());
}

void MainWindow::onRsyncError() {
    outputView->appendPlainText(rsyncProcess->readAllStandardError());
}

void MainWindow::onRsyncFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    QString status = (exitStatus == QProcess::NormalExit && exitCode == 0) ? "Success" : "Failed";
    outputView->appendPlainText(QString("\n--- Process finished with exit code %1 (%2) ---").arg(exitCode).arg(status));
}