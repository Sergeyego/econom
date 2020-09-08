#ifndef DIALOGNEWUNLOAD_H
#define DIALOGNEWUNLOAD_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class DialogNewUnload;
}

class DialogNewUnload : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewUnload(QString nam, QWidget *parent = 0);
    ~DialogNewUnload();
    QString name() const;
    QDate begDate() const;
    QDate endDate() const;

private:
    Ui::DialogNewUnload *ui;
};

#endif // DIALOGNEWUNLOAD_H
