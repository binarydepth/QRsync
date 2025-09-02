// QRsync - A simple Qt-based GUI for the rsync command-line tool.
// Copyright (C) 2025 Carlos J. Checo <binarydepth@gmail.com>
//
// This program is licensed under the Community Public Software License (CPSL) v0.1.
// You should have received a copy of this license along with this program.
// If not, please see the LICENSE.md file in the root directory of this project.

#include "HelpViewer.hpp"
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QFontDatabase>

HelpViewer::HelpViewer(const QString &title, const QString &content, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setMinimumSize(700, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);
    textView = new QPlainTextEdit(this);

    // Use a monospaced font to preserve formatting
    textView->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    // Set to read-only so the user can't edit, but they can still select and copy text
    textView->setReadOnly(true);
    textView->setPlainText(content);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &HelpViewer::reject);

    layout->addWidget(textView);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

HelpViewer::~HelpViewer() = default;