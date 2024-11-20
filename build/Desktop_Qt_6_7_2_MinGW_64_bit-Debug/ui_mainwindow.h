/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *workArea;
    QPushButton *New_Game;
    QLabel *Prompt;
    QLabel *Score;
    QLabel *Speed;
    QLabel *Stopwatch;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1241, 861);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        workArea = new QLabel(centralwidget);
        workArea->setObjectName("workArea");
        workArea->setGeometry(QRect(20, 20, 901, 751));
        workArea->setFrameShape(QFrame::Shape::Box);
        New_Game = new QPushButton(centralwidget);
        New_Game->setObjectName("New_Game");
        New_Game->setGeometry(QRect(1030, 50, 141, 29));
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setUnderline(true);
        New_Game->setFont(font);
        New_Game->setMouseTracking(false);
        Prompt = new QLabel(centralwidget);
        Prompt->setObjectName("Prompt");
        Prompt->setGeometry(QRect(1030, 100, 181, 20));
        Score = new QLabel(centralwidget);
        Score->setObjectName("Score");
        Score->setGeometry(QRect(1030, 160, 181, 20));
        Speed = new QLabel(centralwidget);
        Speed->setObjectName("Speed");
        Speed->setGeometry(QRect(1030, 210, 181, 20));
        Stopwatch = new QLabel(centralwidget);
        Stopwatch->setObjectName("Stopwatch");
        Stopwatch->setGeometry(QRect(1030, 270, 181, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1241, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        workArea->setText(QString());
        New_Game->setText(QCoreApplication::translate("MainWindow", "NEW GAME", nullptr));
        Prompt->setText(QCoreApplication::translate("MainWindow", "Start New Game", nullptr));
        Score->setText(QString());
        Speed->setText(QString());
        Stopwatch->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
