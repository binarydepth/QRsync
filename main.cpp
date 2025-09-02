// QRsync - A simple Qt-based GUI for the rsync command-line tool.
// Copyright (C) 2025 Carlos J. Checo <binarydepth@gmail.com>
//
// This program is licensed under the Community Public Software License (CPSL) v0.1.
// You should have received a copy of this license along with this program.
// If not, please see the LICENSE.md file in the root directory of this project.

#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
