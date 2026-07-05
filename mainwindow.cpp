#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QLabel>
#include<Qdebug>
#include <QFile>
#include <QTimer>
#include<MatchGame.h>
#include<QKeyEvent>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>



MatchGame game;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->titleLabel->setFixedHeight(70);
    QFile file(":/style.qss");

    if(!file.open(QFile::ReadOnly))
        qDebug() << "Error"<<file.errorString();

    this->setStyleSheet(file.readAll());

    initBoard();

    ui->picture_left->setText(
        "<div style='text-align:center;color:#94a3b8;font-size:15px;line-height:1.6;'>"
        "<b style='font-size:20px;color:#e2e8f0;'>2048</b><br><br>"
        "<span style='color:#38bdf8;'>W ↑</span> 上移<br>"
        "<span style='color:#38bdf8;'>S ↓</span> 下移<br>"
        "<span style='color:#38bdf8;'>A ←</span> 左移<br>"
        "<span style='color:#38bdf8;'>D →</span> 右移<br><br>"
        "<span style='color:#64748b;'>合并同值方块</span>"
        "</div>");

    ui->picture_right->setText(
        "<div style='text-align:center;color:#94a3b8;font-size:15px;line-height:1.6;'>"
        "<b style='font-size:20px;color:#e2e8f0;'>Match</b><br><br>"
        "点击选中方块<br>"
        "点击相邻方块<br>"
        "交换并消除<br><br>"
        "<span style='color:#64748b;'>3连以上消除</span>"
        "</div>");

    updateBoard();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateCell(ClickableLabel* label, int value)
{
    if(value == 0)
    {
        label->setText("");

        label->setStyleSheet(
            "background:#334155;"
            "border-radius:8px;"
            );
        return;
    }

    label->setText(QString::number(value));

    QString bgColor;
    QString textColor = "#776e65";
    int fontSize = 28;

    switch(value)
    {
    case 2:
        bgColor = "#eee4da";
        break;

    case 4:
        bgColor = "#ede0c8";
        break;

    case 8:
        bgColor = "#f2b179";
        textColor = "white";
        break;

    case 16:
        bgColor = "#f59563";
        textColor = "white";
        break;

    case 32:
        bgColor = "#f67c5f";
        textColor = "white";
        break;

    case 64:
        bgColor = "#f65e3b";
        textColor = "white";
        break;

    case 128:
        bgColor = "#edcf72";
        textColor = "white";
        break;

    case 256:
        bgColor = "#edcc61";
        textColor = "white";
        break;

    case 512:
        bgColor = "#edc850";
        textColor = "white";
        break;

    case 1024:
        bgColor = "#edc53f";
        textColor = "white";
        fontSize = 22;
        break;

    case 2048:
        bgColor = "#edc22e";
        textColor = "white";
        fontSize = 22;
        break;

    default:
        bgColor = "#3c3a32";
        textColor = "white";
        fontSize = 20;
    }

    label->setStyleSheet(QString(
                             "background:%1;"
                             "border-radius:8px;"
                             "font-size:%2px;"
                             "font-weight:bold;"
                             "font-family:'MapleMono',sans-serif;"
                             "color:%3;"
                             )
                             .arg(bgColor)
                             .arg(fontSize)
                             .arg(textColor));
}

/* =========================
   动画系统
   ========================= */

void MainWindow::saveSnapshot(){
    for(int i=1;i<=N;i++)
        for(int j=1;j<=M;j++)
            prevBoard[i][j] = game.GetBoard(i,j);
}

void MainWindow::animateFadeOut(int row, int col){
    auto *fx = new QGraphicsOpacityEffect(cells[row][col]);
    fx->setOpacity(1.0);
    cells[row][col]->setGraphicsEffect(fx);
    auto *anim = new QPropertyAnimation(fx, "opacity");
    anim->setDuration(180);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    QObject::connect(anim, &QPropertyAnimation::finished, [=](){
        cells[row][col]->setGraphicsEffect(nullptr);
        updateCell(cells[row][col], 0);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animatePopIn(int row, int col){
    updateCell(cells[row][col], game.GetBoard(row,col));
    cells[row][col]->setMaximumSize(0, 0);
    auto *anim = new QPropertyAnimation(cells[row][col], "maximumSize");
    anim->setDuration(220);
    anim->setStartValue(QSize(0, 0));
    anim->setEndValue(QSize(72, 72));
    anim->setEasingCurve(QEasingCurve::OutBack);
    QObject::connect(anim, &QPropertyAnimation::finished, [=](){
        cells[row][col]->setMaximumSize(16777215, 16777215);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animateSwap(int r1, int c1, int r2, int c2){
    ClickableLabel *a = cells[r1][c1], *b = cells[r2][c2];
    QString aStyle = a->styleSheet(), bStyle = b->styleSheet();
    a->setStyleSheet(aStyle + "border:3px solid #f59e0b;");
    b->setStyleSheet(bStyle + "border:3px solid #f59e0b;");
    QTimer::singleShot(150, this, [=](){
        updateCell(a, game.GetBoard(r1,c1));
        updateCell(b, game.GetBoard(r2,c2));
    });
}

void MainWindow::animateAllChanges(){
    // 1. 淡出：prevBoard有值但当前为0 → 被消除了
    for(int i=1;i<=N;i++)
        for(int j=1;j<=M;j++)
            if(prevBoard[i][j] != 0 && game.GetBoard(i,j) == 0)
                animateFadeOut(i, j);

    // 2. 弹入：prevBoard为0但当前有值 → 新生成 / 奖励方块
    QTimer::singleShot(250, this, [this](){
        for(int i=1;i<=N;i++)
            for(int j=1;j<=M;j++)
                if(prevBoard[i][j] == 0 && game.GetBoard(i,j) != 0)
                    animatePopIn(i, j);
        // 其余变动直接刷新（重力下落）
        for(int i=1;i<=N;i++)
            for(int j=1;j<=M;j++)
                if(prevBoard[i][j] != game.GetBoard(i,j) &&
                   !(prevBoard[i][j]==0 && game.GetBoard(i,j)!=0) &&
                   !(prevBoard[i][j]!=0 && game.GetBoard(i,j)==0))
                    updateCell(cells[i][j], game.GetBoard(i,j));
        // 更新分数
        ui->scoreLabel->setText("Score: " + QString::number(game.GetScore()));
    });
}

void MainWindow::initBoard()
{
    QGridLayout* grid =
        qobject_cast<QGridLayout*>(ui->boardWidget->layout());
    if(grid == nullptr)
    {
        qDebug() << "grid layout is null!";
        return;
    }
    for(int i = 1; i <= N; i++)
    {
        for(int j = 1; j <= M; j++)
        {
            cells[i][j]=new ClickableLabel;

            ClickableLabel* label = cells[i][j];

            updateCell(label,0);

            label->setAlignment(Qt::AlignCenter);

            label->setFixedSize(90,90);


            grid->addWidget(label, i-1, j-1);

            connect(label,&ClickableLabel::clicked,this,&MainWindow::on_cell_clicked);
        }
    }
}

void MainWindow::updateBoard()
{
    for(int i = 1; i <= N; i++)
    {
        for(int j = 1; j <= M; j++)
        {
            int value = game.GetBoard(i,j);

            updateCell(cells[i][j],value);
        }
    }

    ui->scoreLabel->setText(
        "Score: " + QString::number(game.GetScore())
        );
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    int dir = -1;
    switch(event->key()){
    case Qt::Key_A:
        dir = 0;
        break;
    case Qt::Key_D:
        dir = 1;
        break;
    case Qt::Key_W:
        dir = 2;
        break;
    case Qt::Key_S:
        dir = 3;
        break;
    default:
        QMainWindow::keyPressEvent(event); 
        return;
    }
    if(game.Move(dir)){
        swap_used = false;
        updateBoard();
        if(!game.IsGameOver()){
            gameover();
        }
    }
}
void MainWindow::on_pushButton_clicked()
{
    game.Init();
    updateBoard();
}

void MainWindow::gameover()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "游戏结束", "游戏结束！是否重新开始？",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        game.Init();
        updateBoard();
    }
    if(reply == QMessageBox::No)
    {
       this->close();
    }
}

void MainWindow::clicked_clear(){
    sel_r = 0;
    sel_c = 0;
    sel_cell = nullptr;
    updateBoard();
}

void MainWindow::on_cell_clicked(){
    //找到被点击的格子(r,c)
    ClickableLabel* clicked = qobject_cast<ClickableLabel*>(sender());
    int r = 0,c = 0;
    for(int i = 1; i<= N; i++){
        for(int j = 1; j <= M; j++){
            if(cells[i][j] == clicked){
                r = i;
                c = j;
            }
        }
    }
    //判断
    //1.第一次点击
    if(sel_cell == nullptr){
        if(game.GetBoard(r, c) == 0) return;
        sel_r = r;
        sel_c = c;
        sel_cell = clicked;
        sel_cell->setStyleSheet(sel_cell->styleSheet() + "border:3px solid #ffcc00;");
    }
    //2.点击同一个格子，视为取消
    else if(sel_r==r && sel_c==c){
        clicked_clear();
    }
    //3.点击相邻格子，触发交换
    else if(abs(sel_r-r)+abs(sel_c-c)==1){
        if(game.GetBoard(r, c) == 0) return;
        if(swap_used){
            clicked_clear();
            return;
        }
        saveSnapshot();
        if(game.update_for_exchange(sel_r, sel_c, r, c)){
            swap_used = true;
            animateSwap(sel_r, sel_c, r, c);
            QTimer::singleShot(200, this, [=](){ animateAllChanges(); });            
        }
        clicked_clear();
        if(!game.IsGameOver()){
            gameover();
        }
    }
    //4.点击不相邻格子，切换选中目标
    else{
        if(game.GetBoard(r, c) == 0) return;
        clicked_clear();
        sel_cell = clicked;
        sel_r = r;
        sel_c = c;
        sel_cell->setStyleSheet(sel_cell->styleSheet() + "border:3px solid #ffcc00;");

    }
}


void MainWindow::on_help_clicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("帮助");
    dialog->resize(450, 450);

    QLabel *imageLabel = new QLabel(dialog);

    QPixmap pix(":/help.jpg");   // Qt资源文件里的图片

    imageLabel->setPixmap(
        pix.scaled(550, 550,
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation));

    imageLabel->setAlignment(Qt::AlignCenter);


    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(imageLabel);

    dialog->setLayout(layout);
    dialog->exec();
}


void MainWindow::on_thanks_clicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->resize(450, 450);

    QLabel *imageLabel = new QLabel(dialog);

    QPixmap pix(":/thanks.jpg");

    imageLabel->setPixmap(
        pix.scaled(550, 550,
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation));

    imageLabel->setAlignment(Qt::AlignCenter);


    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(imageLabel);

    dialog->setLayout(layout);
    dialog->exec();
}
