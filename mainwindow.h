#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QLabel>
#include<Game.h>
#include <QMainWindow>
#include<QKeyEvent>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void initBoard();
    void updateBoard();
    QLabel* cells[N+1][M+1];

private:
    Ui::MainWindow *ui;
protected:
    void keyPressEvent(QKeyEvent *event) override;    
};
#endif // MAINWINDOW_H
