// QRsync - A simple Qt-based GUI for the rsync command-line tool.
// Copyright (C) 2025 Carlos J. Checo <binarydepth@gmail.com>
//
// This program is licensed under the Community Public Software License (CPSL) v0.1.
// You should have received a copy of this license along with this program.
// If not, please see the LICENSE.md file in the root directory of this project.

#ifndef HELPVIEWER_HPP
#define HELPVIEWER_HPP

#include <QDialog>

class QPlainTextEdit;

class HelpViewer : public QDialog
{
    Q_OBJECT

public:
    explicit HelpViewer(const QString &title, const QString &content, QWidget *parent = nullptr);
    ~HelpViewer() override;

private:
    QPlainTextEdit *textView;
};

#endif // HELPVIEWER_HPP