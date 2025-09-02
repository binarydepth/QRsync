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