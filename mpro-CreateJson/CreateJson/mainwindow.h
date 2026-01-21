#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setPhase();
    void setTem();
    void setOutset();
    void setupTable(QTableWidget *, int , int, const QStringList &, const QVector<QVector<QString>> &);
protected:
    void createPhase();
    void createLoop();
    void createTem();
    void createOutput();
    QString getTopic(int);
    QString getSubtopic(int);
    void createJsonFromRow(QTableWidget *table, int row);

private slots:
    void on_OutsetBtn_clicked();

    void on_CreateJsonBtn_clicked();

private:
    QStringList headers;
    Ui::MainWindow *ui;
    QVector<QVector<QString>> initValues;
};
#endif // MAINWINDOW_H
