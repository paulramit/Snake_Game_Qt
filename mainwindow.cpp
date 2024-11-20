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
    timer->start(interval);

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
    // Check if level and difficulty are selected
    if (!(ui->Mode_1->isChecked() || ui->Mode_2->isChecked() || ui->Mode_3->isChecked())) {
        ui->Prompt->setText("Select a Level to Start the Game.");
        return;
    }
    if (!(ui->Easy->isChecked() || ui->Medium->isChecked() || ui->Hard->isChecked())) {
        ui->Prompt->setText("Select a Difficulty to Start the Game.");
        return;
    }

    // Set the interval based on selected difficulty
    int interval = 0;
    if (ui->Easy->isChecked()) {
        interval = 100;
    } else if (ui->Medium->isChecked()) {
        interval = 80;
    } else if (ui->Hard->isChecked()) {
        interval = 60;
    }

    // Restart the game timer with the new interval
    timer->start(interval);

    // Initialize game parameters
    width = ui->workArea->width();
    height = ui->workArea->height();
    centerX = width / 2;
    centerY = height / 2;
    score = 0;

    // Update UI components
    ui->Prompt->setText("Press Enter to Start");
    ui->Score->setText("Score: " + QString::number(static_cast<int>(score)));
    ui->Stopwatch->setText("00:00:00");

    // Clear the canvas and reset snake data
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

    // Reset game state
    direction = {0, 0};
    score = 0;
    food = {INT_MAX, INT_MAX};
    started = 0;
    elapsedTime = 0;

    // Add walls for Mode_2
    if (ui->Mode_2->isChecked()) {
        createBoundaryWalls();
    }
}

// Function to create boundary walls
void MainWindow::createBoundaryWalls() {
    walls.clear(); // Clear any existing walls

    int outerXStart = 0, outerXEnd = width - 1;
    int outerYStart = 0, outerYEnd = height - 1;
    int innerXStart = 2, innerXEnd = width - 3;
    int innerYStart = 2, innerYEnd = height - 3;

    // Deep orange color
    int r = 255, g = 140, b = 0;

    // Add points for the outer rectangle
    for (int x = outerXStart; x <= outerXEnd; x+=gridOffset) {
        for (int i = 0; i < 2; ++i) { // Top and Bottom rows
            walls.insert(QPoint(x, outerYStart + i));
            walls.insert(QPoint(x, outerYEnd - i));
            colorPointAbsolute(x, outerYStart + i, r, g, b, gridOffset);
            Delay;
            colorPointAbsolute(x, outerYEnd - i, r, g, b, gridOffset);
            Delay;
        }
    }
    for (int y = outerYStart; y <= outerYEnd; y+=gridOffset) {
        for (int i = 0; i < 2; ++i) { // Left and Right columns
            walls.insert(QPoint(outerXStart + i, y));
            walls.insert(QPoint(outerXEnd - i, y));
            colorPointAbsolute(outerXStart + i, y, r, g, b, gridOffset);
            Delay;
            colorPointAbsolute(outerXEnd - i, y, r, g, b, gridOffset);
            Delay;
        }
    }

    // Add points for the inner rectangle
    for (int x = innerXStart; x <= innerXEnd; x+=gridOffset) {
        for (int i = 0; i < 2; ++i) { // Top and Bottom rows
            walls.insert(QPoint(x, innerYStart + i));
            walls.insert(QPoint(x, innerYEnd - i));
            colorPointAbsolute(x, innerYStart + i, r, g, b, gridOffset);
            Delay;
            colorPointAbsolute(x, innerYEnd - i, r, g, b, gridOffset);
            Delay;
        }
    }
    for (int y = innerYStart; y <= innerYEnd; y+=gridOffset) {
        for (int i = 0; i < 2; ++i) { // Left and Right columns
            walls.insert(QPoint(innerXStart + i, y));
            walls.insert(QPoint(innerXEnd - i, y));
            colorPointAbsolute(innerXStart + i, y, r, g, b, gridOffset);
            Delay;
            colorPointAbsolute(innerXEnd - i, y, r, g, b, gridOffset);
            Delay;
        }
    }
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
