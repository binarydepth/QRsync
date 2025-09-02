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