#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QLabel>
#include<Game.h>
#include <QMainWindow>
#include<QKeyEvent>
#include<ClickableLabel.h>
#include <QMessageBox>


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
    void updateCell(ClickableLabel* label, int value);
    ClickableLabel* cells[N+1][M+1];
    void gameover();
    void clicked_clear();

    // 动画系统
    int prevBoard[N+1][M+1];
    void saveSnapshot();
    void animateFadeOut(int row, int col);
    void animatePopIn(int row, int col);
    void animateSwap(int r1, int c1, int r2, int c2);
    void animateAllChanges();
    void animateSlide(int direction);
private:
    Ui::MainWindow *ui;
    ClickableLabel* sel_cell = nullptr;
    int sel_r = 0;
    int sel_c = 0;
    bool swap_used = false;
protected:
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void on_pushButton_clicked();
    void on_cell_clicked();
    void on_help_clicked();
    void on_thanks_clicked();

};
#endif // MAINWINDOW_H
