#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QLabel>
#include<Qdebug>
#include <QFile>
#include<MatchGame.h>


MatchGame game;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile file(":/style.qss");

    file.open(QFile::ReadOnly);

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
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
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
                "color:white;"
                );

            grid->addWidget(label, i, j);

        }
    }
}

void MainWindow::updateBoard()
{
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            int value = game.Board[i][j];

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
        "Score: " + QString::number(game.score)
        );
}
