#include "MainWindow.hpp"
#include <QtWidgets>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), rsyncProcess(nullptr) {
    setupUI();

    // Set up settings path
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if (!configDir.exists()) {
        configDir.mkpath(".");
    }
    settingsFilePath = configDir.filePath("qrsync_syncsets.json");

    populateSyncsetMenus(); // Initial population of the menus

    rsyncProcess = new QProcess(this);
    connect(rsyncProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onRsyncOutput);
    connect(rsyncProcess, &QProcess::readyReadStandardError, this, &MainWindow::onRsyncError);
    connect(rsyncProcess, &QProcess::finished, this, &MainWindow::onRsyncFinished);

    // Initial button state
    stopButton->setEnabled(false);

    // Trigger the initial state for the archive options
    onArchiveToggled(archiveCheck->isChecked());
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("QRsync");
    resize(800, 700);

    setupMenuBar();

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

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
    archiveCheck->setChecked(true);
    connect(archiveCheck, &QCheckBox::toggled, this, &MainWindow::onArchiveToggled);
    optionsLayout->addWidget(archiveCheck, 0, 0, 1, 2); // Span 2 columns

    // Sub-options
    recursiveCheck = new QCheckBox("Recursive (-r)");
    symlinksCheck = new QCheckBox("Copy symlinks (-l)");
    permsCheck = new QCheckBox("Preserve permissions (-p)");
    timesCheck = new QCheckBox("Preserve times (-t)");
    groupCheck = new QCheckBox("Preserve group (-g)");
    ownerCheck = new QCheckBox("Preserve owner (-o)");

    QWidget *subOptionsWidget = new QWidget();
    QGridLayout *subOptionsLayout = new QGridLayout(subOptionsWidget);
    subOptionsLayout->setContentsMargins(20, 0, 0, 0); // Indent
    subOptionsLayout->addWidget(recursiveCheck, 0, 0);
    subOptionsLayout->addWidget(symlinksCheck, 0, 1);
    subOptionsLayout->addWidget(permsCheck, 1, 0);
    subOptionsLayout->addWidget(timesCheck, 1, 1);
    subOptionsLayout->addWidget(groupCheck, 2, 0);
    subOptionsLayout->addWidget(ownerCheck, 2, 1);
    optionsLayout->addWidget(subOptionsWidget, 1, 0, 1, 4);

    optionsLayout->addWidget(new QFrame(), 2, 0); // Spacer

    // Other standard options
    verboseCheck = new QCheckBox("Verbose (-v)");
    verboseCheck->setChecked(true);
    progressCheck = new QCheckBox("Progress (--progress)");
    progressCheck->setChecked(true);
    deleteCheck = new QCheckBox("Delete on destination (--delete)");
    sizeOnlyCheck = new QCheckBox("Size only (--size-only)");
    ignoreExistingCheck = new QCheckBox("Ignore existing (--ignore-existing)");
    skipNewerCheck = new QCheckBox("Skip newer (--update)");

    optionsLayout->addWidget(verboseCheck, 3, 0);
    optionsLayout->addWidget(progressCheck, 3, 1);
    optionsLayout->addWidget(skipNewerCheck, 3, 2);
    optionsLayout->addWidget(deleteCheck, 4, 0);
    optionsLayout->addWidget(sizeOnlyCheck, 4, 1);
    optionsLayout->addWidget(ignoreExistingCheck, 4, 2);

    mainLayout->addWidget(optionsGroup);

    // --- Manual Options Group ---
    QGroupBox *manualGroup = new QGroupBox("Manual Options");
    QVBoxLayout *manualLayout = new QVBoxLayout(manualGroup);
    manualOptionsEdit = new QLineEdit();
    manualOptionsEdit->setPlaceholderText("--exclude=*.tmp --bwlimit=1000");
    manualLayout->addWidget(manualOptionsEdit);
    mainLayout->addWidget(manualGroup);

    // --- Output Group ---
    QGroupBox *outputGroup = new QGroupBox("Output");
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    outputView = new QPlainTextEdit();
    outputView->setReadOnly(true);
    outputView->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    outputLayout->addWidget(outputView);
    mainLayout->addWidget(outputGroup);

    // --- Buttons ---
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

    modeActionGroup = new QActionGroup(this);
    modeActionGroup->setExclusive(true);
    contentsAction = new QAction("Contents", this);
    contentsAction->setCheckable(true);
    contentsAction->setChecked(true);
    connect(contentsAction, &QAction::triggered, this, &MainWindow::onModeContents);
    modeActionGroup->addAction(contentsAction);
    mirrorAction = new QAction("Mirror", this);
    mirrorAction->setCheckable(true);
    connect(mirrorAction, &QAction::triggered, this, &MainWindow::onModeMirror);
    modeActionGroup->addAction(mirrorAction);
    appMenu->addAction(contentsAction);
    appMenu->addAction(mirrorAction);
    appMenu->addSeparator();

    manualAction = new QAction("Manual", this);
    manualAction->setCheckable(true);
    connect(manualAction, &QAction::toggled, this, &MainWindow::onManualModeToggled);
    appMenu->addAction(manualAction);
    appMenu->addSeparator();

    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, &QApplication::quit);
    appMenu->addAction(quitAction);

    QMenu *syncsetMenu = menuBar()->addMenu("&Syncsets");
    syncsetMenu->addAction("New", this, &MainWindow::onNew);
    loadMenu = syncsetMenu->addMenu("Load");
    saveMenu = syncsetMenu->addMenu("Save");
    syncsetMenu->addSeparator();
    renameMenu = syncsetMenu->addMenu("Rename");
    deleteMenu = syncsetMenu->addMenu("Delete");

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("About QRsync", this, &MainWindow::onAbout);
}

void MainWindow::populateSyncsetMenus() {
    loadMenu->clear();
    saveMenu->clear();
    renameMenu->clear();
    deleteMenu->clear();

    QJsonObject syncsets = loadSyncsets();
    QStringList names = syncsets.keys();
    names.sort(Qt::CaseInsensitive);

    if (names.isEmpty()) {
        auto createEmptyAction = [this]() {
            QAction *action = new QAction("No saved Syncsets", this);
            action->setEnabled(false);
            return action;
        };
        loadMenu->addAction(createEmptyAction());
        saveMenu->addAction(createEmptyAction());
        renameMenu->addAction(createEmptyAction());
        deleteMenu->addAction(createEmptyAction());
    } else {
        for (const QString &name : names) {
            QAction *loadAction = new QAction(name, this);
            connect(loadAction, &QAction::triggered, this, [this, name]() { onLoad(name); });
            loadMenu->addAction(loadAction);

            QAction *saveAction = new QAction(name, this);
            connect(saveAction, &QAction::triggered, this, [this, name]() { onSave(name); });
            saveMenu->addAction(saveAction);

            QAction *renameAction = new QAction(name, this);
            connect(renameAction, &QAction::triggered, this, [this, name]() { onRename(name); });
            renameMenu->addAction(renameAction);

            QAction *deleteAction = new QAction(name, this);
            connect(deleteAction, &QAction::triggered, this, [this, name]() { onDelete(name); });
            deleteMenu->addAction(deleteAction);
        }
    }
}

void MainWindow::applySyncset(const QJsonObject &syncset) {
    sourceEdit->setText(syncset["source"].toString());
    destinationEdit->setText(syncset["destination"].toString());

    if (sourceEdit->text().endsWith('/')) {
        contentsAction->setChecked(true);
    } else {
        mirrorAction->setChecked(true);
    }

    QJsonObject options = syncset["options"].toObject();

    manualAction->setChecked(options.contains("manual_mode") ? options.value("manual_mode").toBool() : false);

    archiveCheck->setChecked(options.contains("archive") ? options.value("archive").toBool() : true);

    recursiveCheck->setChecked(options.contains("recursive") ? options.value("recursive").toBool() : true);
    symlinksCheck->setChecked(options.contains("symlinks") ? options.value("symlinks").toBool() : false);
    permsCheck->setChecked(options.contains("perms") ? options.value("perms").toBool() : false);
    timesCheck->setChecked(options.contains("times") ? options.value("times").toBool() : false);
    groupCheck->setChecked(options.contains("group") ? options.value("group").toBool() : false);
    ownerCheck->setChecked(options.contains("owner") ? options.value("owner").toBool() : false);

    verboseCheck->setChecked(options.contains("verbose") ? options.value("verbose").toBool() : true);
    progressCheck->setChecked(options.contains("progress") ? options.value("progress").toBool() : true);
    deleteCheck->setChecked(options.contains("delete") ? options.value("delete").toBool() : false);
    sizeOnlyCheck->setChecked(options.contains("sizeOnly") ? options.value("sizeOnly").toBool() : false);
    ignoreExistingCheck->setChecked(options.contains("ignoreExisting") ? options.value("ignoreExisting").toBool() : false);
    skipNewerCheck->setChecked(options.contains("skipNewer") ? options.value("skipNewer").toBool() : false);
    manualOptionsEdit->setText(options.contains("manual_options") ? options.value("manual_options").toString() : "");

    // Manually trigger the logic to update the UI state based on loaded modes
    onManualModeToggled(manualAction->isChecked());
    // This second call ensures the sub-options are correctly updated after being loaded
    onArchiveToggled(archiveCheck->isChecked());
}


// --- Slots Implementation ---

void MainWindow::onManualModeToggled(bool checked) {
    if (checked) {
        // --- ENTERING MANUAL MODE ---
        // Enable all sub-options so the user can control them.
        QList<QCheckBox*> subOptions = {
            recursiveCheck, symlinksCheck, permsCheck,
            timesCheck, groupCheck, ownerCheck
        };
        for (QCheckBox* checkbox : subOptions) {
            checkbox->setEnabled(true);
        }
    } else {
        // --- LEAVING MANUAL MODE ---
        // Re-apply the standard archive logic to restore a safe state.
        onArchiveToggled(archiveCheck->isChecked());
    }
}

void MainWindow::onArchiveToggled(bool checked) {
    // In manual mode, this logical link is broken.
    if (manualAction->isChecked()) {
        return;
    }

    QList<QCheckBox*> subOptions = {
        recursiveCheck, symlinksCheck, permsCheck,
        timesCheck, groupCheck, ownerCheck
    };

    for (QCheckBox* checkbox : subOptions) {
        checkbox->setEnabled(!checked);
        checkbox->setChecked(checked);
    }

    if (!checked) {
        // When leaving archive mode, 'recursive' is a sensible default to keep checked.
        recursiveCheck->setChecked(true);
    }
}

void MainWindow::onModeContents() {
    QString currentSource = sourceEdit->text();
    if (!currentSource.isEmpty() && !currentSource.endsWith('/')) {
        sourceEdit->setText(currentSource + "/");
    }
}

void MainWindow::onModeMirror() {
    QString currentSource = sourceEdit->text();
    if (currentSource.endsWith('/')) {
        sourceEdit->setText(currentSource.chopped(1));
    }
    QString currentDest = destinationEdit->text();
    if (currentDest.endsWith('/')) {
        destinationEdit->setText(currentDest.chopped(1));
    }
}

void MainWindow::onBrowseSource() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Source Directory");
    if (!directory.isEmpty()) {
        if (contentsAction->isChecked()) {
            sourceEdit->setText(directory + "/");
        } else {
            sourceEdit->setText(directory);
        }
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

    if (manualAction->isChecked()) {
        // Manual Mode: Build from individual checkboxes, ignoring archiveCheck's state
        if (archiveCheck->isChecked()) arguments << "-a";
        if (recursiveCheck->isChecked()) arguments << "-r";
        if (symlinksCheck->isChecked()) arguments << "-l";
        if (permsCheck->isChecked()) arguments << "-p";
        if (timesCheck->isChecked()) arguments << "-t";
        if (groupCheck->isChecked()) arguments << "-g";
        if (ownerCheck->isChecked()) arguments << "-o";
    } else {
        // Normal Mode: Standard logic
        if (archiveCheck->isChecked()) {
            arguments << "-a";
        } else {
            // Build from the visible and enabled sub-options
            if (recursiveCheck->isChecked()) arguments << "-r";
            if (symlinksCheck->isChecked()) arguments << "-l";
            if (permsCheck->isChecked()) arguments << "-p";
            if (timesCheck->isChecked()) arguments << "-t";
            if (groupCheck->isChecked()) arguments << "-g";
            if (ownerCheck->isChecked()) arguments << "-o";
        }
    }

    // These options are independent of archive/manual mode
    if (verboseCheck->isChecked()) arguments << "-v";
    if (progressCheck->isChecked()) arguments << "--progress";
    if (sizeOnlyCheck->isChecked()) arguments << "--size-only";
    if (ignoreExistingCheck->isChecked()) arguments << "--ignore-existing";
    if (skipNewerCheck->isChecked()) arguments << "--update";
    if (deleteCheck->isChecked()) arguments << "--delete";

    // Add manual options text in BOTH modes
    QString manualOpts = manualOptionsEdit->text();
    arguments.append(manualOpts.split(" ", Qt::SkipEmptyParts));

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

void MainWindow::onNew() {
    bool ok;
    QString name = QInputDialog::getText(this, "New Syncset", "Enter a name for this Syncset:", QLineEdit::Normal, "", &ok);

    if (ok && !name.isEmpty()) {
        QJsonObject syncsets = loadSyncsets();
        if (syncsets.contains(name)) {
            QMessageBox::warning(this, "Name Exists", "A Syncset with this name already exists.");
            return;
        }

        QJsonObject newSyncset;
        newSyncset["source"] = sourceEdit->text();
        newSyncset["destination"] = destinationEdit->text();

        QJsonObject options;
        options["manual_mode"] = manualAction->isChecked();
        options["archive"] = archiveCheck->isChecked();
        options["recursive"] = recursiveCheck->isChecked();
        options["symlinks"] = symlinksCheck->isChecked();
        options["perms"] = permsCheck->isChecked();
        options["times"] = timesCheck->isChecked();
        options["group"] = groupCheck->isChecked();
        options["owner"] = ownerCheck->isChecked();
        options["verbose"] = verboseCheck->isChecked();
        options["progress"] = progressCheck->isChecked();
        options["delete"] = deleteCheck->isChecked();
        options["sizeOnly"] = sizeOnlyCheck->isChecked();
        options["ignoreExisting"] = ignoreExistingCheck->isChecked();
        options["skipNewer"] = skipNewerCheck->isChecked();
        options["manual_options"] = manualOptionsEdit->text();
        newSyncset["options"] = options;

        syncsets[name] = newSyncset;
        saveSyncsets(syncsets);
        populateSyncsetMenus();
        QMessageBox::information(this, "Success", "Syncset '" + name + "' saved successfully.");
    }
}

void MainWindow::onLoad(const QString &name) {
    QJsonObject syncsets = loadSyncsets();
    if (syncsets.contains(name)) {
        applySyncset(syncsets[name].toObject());
        statusBar()->showMessage("Loaded '" + name + "'.", 3000);
    }
}

void MainWindow::onSave(const QString &name) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Save", "Overwrite '" + name + "' with the current settings?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QJsonObject syncsets = loadSyncsets();

        QJsonObject updatedSyncset;
        updatedSyncset["source"] = sourceEdit->text();
        updatedSyncset["destination"] = destinationEdit->text();

        QJsonObject options;
        options["manual_mode"] = manualAction->isChecked();
        options["archive"] = archiveCheck->isChecked();
        options["recursive"] = recursiveCheck->isChecked();
        options["symlinks"] = symlinksCheck->isChecked();
        options["perms"] = permsCheck->isChecked();
        options["times"] = timesCheck->isChecked();
        options["group"] = groupCheck->isChecked();
        options["owner"] = ownerCheck->isChecked();
        options["verbose"] = verboseCheck->isChecked();
        options["progress"] = progressCheck->isChecked();
        options["delete"] = deleteCheck->isChecked();
        options["sizeOnly"] = sizeOnlyCheck->isChecked();
        options["ignoreExisting"] = ignoreExistingCheck->isChecked();
        options["skipNewer"] = skipNewerCheck->isChecked();
        options["manual_options"] = manualOptionsEdit->text();
        updatedSyncset["options"] = options;

        syncsets[name] = updatedSyncset;
        saveSyncsets(syncsets);
        statusBar()->showMessage("Saved '" + name + "'.", 3000);
    }
}

void MainWindow::onRename(const QString &oldName) {
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename Syncset", "Enter the new name for '" + oldName + "':", QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty()) {
        if (oldName == newName) { return; }

        QJsonObject syncsets = loadSyncsets();
        if (syncsets.contains(newName)) {
            QMessageBox::warning(this, "Name Exists", "A Syncset with the name '" + newName + "' already exists.");
            return;
        }

        QJsonValue value = syncsets.take(oldName);
        syncsets.insert(newName, value);
        saveSyncsets(syncsets);
        populateSyncsetMenus();
        statusBar()->showMessage("Renamed '" + oldName + "' to '" + newName + "'.", 3000);
    }
}

void MainWindow::onDelete(const QString &name) {
    QJsonObject syncsets = loadSyncsets();
    syncsets.remove(name);
    saveSyncsets(syncsets);
    populateSyncsetMenus();
    statusBar()->showMessage("Deleted '" + name + "'.", 3000);
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About QRsync",
                       "<h3>QRsync</h3>"
                       "<p>A simple Qt-based GUI for the rsync command-line tool.</p>"
                       "<p>Version 0.2</p>");
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

QJsonObject MainWindow::loadSyncsets() {
    QFile file(settingsFilePath);
    if (!file.open(QIODevice::ReadOnly)) { return QJsonObject(); }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
}

void MainWindow::saveSyncsets(const QJsonObject &syncsets) {
    QFile file(settingsFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning("Couldn't open settings file for writing.");
        return;
    }
    QJsonDocument doc(syncsets);
    file.write(doc.toJson(QJsonDocument::Indented));
}