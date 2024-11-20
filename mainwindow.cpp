#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>

#define Delay delay(1)
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->workArea->setFocusPolicy(Qt::StrongFocus);
    ui->workArea->setFocus();

    QPixmap canvas = ui->workArea->pixmap(Qt::ReturnByValue);
    if (canvas.isNull()) {
        canvas = QPixmap(ui->workArea->size());
        canvas.fill(Qt::white);
        ui->workArea->setPixmap(canvas);
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::moveSnake);
    timer->start(75);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateWatch);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::delay(int ms){
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

void MainWindow::colorPointAbsolute(int x, int y, int r, int g, int b, int penwidth) {
    QPixmap canvas = ui->workArea->pixmap();
    QPainter painter(&canvas);
    QPen pen = QPen(QColor(r, g, b), penwidth);
    painter.setPen(pen);
    painter.drawPoint(x, y);
    ui->workArea->setPixmap(canvas);
}

void MainWindow::colorPointRelative(int x, int y, int r, int g, int b) {
    int absX = centerX + x * gridOffset + gridOffset / 2;
    int absY = centerY - y * gridOffset - gridOffset / 2;
    colorPointAbsolute(absX, absY, r, g, b, gridOffset);
}

void MainWindow::on_New_Game_clicked() {
    width = ui->workArea->width();
    height = ui->workArea->height();
    qDebug()<<width<<' '<<height;
    centerX = width / 2;
    centerY = height / 2;
    score = 0;
    ui->Prompt->setText("Press Enter to Start");
    ui->Score->setText("Score: " + QString::number(static_cast<int>(score)));

    QPixmap canvas = ui->workArea->pixmap();
    canvas.fill(Qt::white);
    ui->workArea->setPixmap(canvas);

    snake.clear();
    snakePoints.clear();
    for (int i = -2; i < 3; i++) {
        snake.push_back({i, 0});
        snakePoints.insert({i, 0});
        colorPointRelative(i, 0, 0, 0, 0);
    }

    direction = {0, 0};
    score = 0;
    speed = 1;
    food = {INT_MAX, INT_MAX};
    started = 0;
    elapsedTime=0;
    ui->Stopwatch->setText("00:00:00");
}

QColor MainWindow::getPixelColor(int x, int y) {
    QImage image = ui->workArea->pixmap(Qt::ReturnByValue).toImage();
    return image.pixelColor(x, y);
}

bool MainWindow::inSnake(int x, int y) {
    return snakePoints.contains({x, y});
}

void MainWindow::growFood() {
    int x, y;
    do {
        x = (rand() % (width / gridOffset)) - (width / (2 * gridOffset));
        y = (rand() % (height / gridOffset)) - (height / (2 * gridOffset));
    } while (inSnake(x, y));

    food = QPoint(x, y);
    colorPointRelative(x, y, 255, 0, 0);
}

void MainWindow::moveSnake() {
    if (direction[0] == 0 && direction[1] == 0)
        return;

    if (food == QPoint(INT_MAX, INT_MAX))
        growFood();

    QPoint head = snake.back();
    int nextX = head.x() + direction[0], nextY = head.y() + direction[1];

    // Wrap around if the snake goes beyond the edges of the window
    if (nextX < -(width / (2 * gridOffset)))
        nextX = width / (2 * gridOffset) - 1;
    else if (nextX >= width / (2 * gridOffset))
        nextX = -(width / (2 * gridOffset));

    if (nextY < -(height / (2 * gridOffset)))
        nextY = height / (2 * gridOffset) - 1;
    else if (nextY >= height / (2 * gridOffset))
        nextY = -(height / (2 * gridOffset));

    /*if (absX < 0 || absX > width || absY < 0 || absY > height || snakePoints.contains({nextX, nextY})) {
        ui->Prompt->setText("Game Over");
        direction = {0, 0};
        started = -1;
        return;
    }*/
    if (snakePoints.contains({nextX, nextY})) {
        ui->Prompt->setText("Game Over");
        direction = {0, 0};
        started = -1;
        gameTimer->stop();
        return;
    }

    snake.push_back({nextX, nextY});
    snakePoints.insert({nextX, nextY});
    colorPointRelative(nextX, nextY, 0, 0, 0);

    if (food == QPoint(nextX, nextY)) {
        //speed += 0.1;
        score += 1;
        ui->Score->setText("Score: " + QString::number(static_cast<int>(score)));
        //ui->Speed->setText("Speed Multiplier: " + QString::number(speed, 'f', 1) + "x");
        growFood();
    } else {
        QPoint tail = snake.front();
        colorPointRelative(tail.x(), tail.y(), 255, 255, 255);
        snake.pop_front();
        snakePoints.erase(snakePoints.find(tail));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();

    if (started == 0 && key == Qt::Key_Enter) {
        direction = {1, 0};
        started = 1;
        ui->Prompt->setText("Game Started");
        gameTimer->start(1000);
    }

    if (started == 1) {
        QVector<int> newDirection = direction;

        if (key == Qt::Key_Right && !(direction[0] == -1 && direction[1] == 0)) {
            newDirection = {1, 0};
        } else if (key == Qt::Key_Left && !(direction[0] == 1 && direction[1] == 0)) {
            newDirection = {-1, 0};
        } else if (key == Qt::Key_Up && !(direction[0] == 0 && direction[1] == -1)) {
            newDirection = {0, 1};
        } else if (key == Qt::Key_Down && !(direction[0] == 0 && direction[1] == 1)) {
            newDirection = {0, -1};
        }

        if (newDirection != direction) {
            direction = newDirection;
        }
    }
}

void MainWindow::updateWatch() {
    elapsedTime++;
    int hours = elapsedTime / 3600;
    int minutes = (elapsedTime % 3600) / 60;
    int seconds = elapsedTime % 60;

    QString timeString = QString("%1:%2:%3")
                             .arg(hours, 2, 10, QChar('0'))
                             .arg(minutes, 2, 10, QChar('0'))
                             .arg(seconds, 2, 10, QChar('0'));

    ui->Stopwatch->setText(timeString);  // Update the label with the new time
}
