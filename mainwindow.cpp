#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QLabel>
#include<Qdebug>
#include <QFile>
#include<MatchGame.h>
#include<QKeyEvent>


MatchGame game;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile file(":/style.qss");

    if(!file.open(QFile::ReadOnly))
        qDebug() << "Error"<<file.errorString();

    this->setStyleSheet(file.readAll());

    initBoard();

    updateBoard();
}

MainWindow::~MainWindow()
{
    delete ui;
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
            cells[i][j]=new QLabel;

            QLabel* label = cells[i][j];

            label->setText("0");

            label->setAlignment(Qt::AlignCenter);

            label->setFixedSize(72,72);

            label->setStyleSheet(
                "background:#dbeafe;"
                "border-radius:10px;"
                "font-size:24px;"
                "font-weight:bold;"
                "color:black;"
                );

            grid->addWidget(label, i-1, j-1);

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

            if(value == 0)
            {
                cells[i][j]->setText("");
            }
            else
            {
                cells[i][j]->setText(
                    QString::number(value)
                    );
            }
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
    }
    if(game.Move(dir)){
        game.chain_match();
        updateBoard();
    }
}