/********************************************************************************
** Form generated from reading UI file 'qt1.ui'
**
** Created: Thu Apr 13 22:12:19 2023
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT1_H
#define UI_QT1_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Qt1
{
public:
    QLabel *lb_time;
    QLabel *lb_pic;
    QLabel *label_camera;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pb_prev;
    QPushButton *pb_next;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pb_history;
    QPushButton *pb_del;
    QPushButton *pb_open;
    QLabel *label_photo;
    QPushButton *pb_start;
    QTextBrowser *text_time;
    QComboBox *comboBox;
    QTextBrowser *text_R;
    QLabel *label_R;
    QLabel *label_Rwarning;
    QTextBrowser *text_Rwarning;
    QLabel *lb_num;
    QLabel *lb_bj;

    void setupUi(QDialog *Qt1)
    {
        if (Qt1->objectName().isEmpty())
            Qt1->setObjectName(QString::fromUtf8("Qt1"));
        Qt1->resize(480, 272);
        lb_time = new QLabel(Qt1);
        lb_time->setObjectName(QString::fromUtf8("lb_time"));
        lb_time->setGeometry(QRect(10, 240, 171, 20));
        lb_pic = new QLabel(Qt1);
        lb_pic->setObjectName(QString::fromUtf8("lb_pic"));
        lb_pic->setGeometry(QRect(10, 10, 271, 201));
        label_camera = new QLabel(Qt1);
        label_camera->setObjectName(QString::fromUtf8("label_camera"));
        label_camera->setGeometry(QRect(290, 10, 121, 31));
        QFont font;
        font.setFamily(QString::fromUtf8("Ubuntu Condensed"));
        font.setBold(true);
        font.setWeight(75);
        label_camera->setFont(font);
        layoutWidget = new QWidget(Qt1);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(290, 190, 181, 33));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pb_prev = new QPushButton(layoutWidget);
        pb_prev->setObjectName(QString::fromUtf8("pb_prev"));
        pb_prev->setFont(font);

        horizontalLayout->addWidget(pb_prev);

        pb_next = new QPushButton(layoutWidget);
        pb_next->setObjectName(QString::fromUtf8("pb_next"));
        pb_next->setFont(font);

        horizontalLayout->addWidget(pb_next);

        layoutWidget1 = new QWidget(Qt1);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(200, 230, 271, 33));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        pb_history = new QPushButton(layoutWidget1);
        pb_history->setObjectName(QString::fromUtf8("pb_history"));
        pb_history->setFont(font);

        horizontalLayout_2->addWidget(pb_history);

        pb_del = new QPushButton(layoutWidget1);
        pb_del->setObjectName(QString::fromUtf8("pb_del"));
        pb_del->setFont(font);

        horizontalLayout_2->addWidget(pb_del);

        pb_open = new QPushButton(layoutWidget1);
        pb_open->setObjectName(QString::fromUtf8("pb_open"));
        pb_open->setFont(font);

        horizontalLayout_2->addWidget(pb_open);

        label_photo = new QLabel(Qt1);
        label_photo->setObjectName(QString::fromUtf8("label_photo"));
        label_photo->setGeometry(QRect(290, 160, 131, 31));
        label_photo->setFont(font);
        pb_start = new QPushButton(Qt1);
        pb_start->setObjectName(QString::fromUtf8("pb_start"));
        pb_start->setGeometry(QRect(290, 50, 81, 31));
        pb_start->setFont(font);
        text_time = new QTextBrowser(Qt1);
        text_time->setObjectName(QString::fromUtf8("text_time"));
        text_time->setGeometry(QRect(410, 50, 61, 31));
        text_time->setFont(font);
        comboBox = new QComboBox(Qt1);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(410, 20, 61, 21));
        comboBox->setFont(font);
        text_R = new QTextBrowser(Qt1);
        text_R->setObjectName(QString::fromUtf8("text_R"));
        text_R->setGeometry(QRect(290, 120, 71, 31));
        text_R->setFont(font);
        label_R = new QLabel(Qt1);
        label_R->setObjectName(QString::fromUtf8("label_R"));
        label_R->setGeometry(QRect(290, 100, 21, 21));
        label_R->setFont(font);
        label_Rwarning = new QLabel(Qt1);
        label_Rwarning->setObjectName(QString::fromUtf8("label_Rwarning"));
        label_Rwarning->setGeometry(QRect(400, 100, 51, 21));
        label_Rwarning->setFont(font);
        text_Rwarning = new QTextBrowser(Qt1);
        text_Rwarning->setObjectName(QString::fromUtf8("text_Rwarning"));
        text_Rwarning->setGeometry(QRect(400, 120, 71, 31));
        text_Rwarning->setFont(font);
        lb_num = new QLabel(Qt1);
        lb_num->setObjectName(QString::fromUtf8("lb_num"));
        lb_num->setGeometry(QRect(10, 220, 51, 21));
        lb_bj = new QLabel(Qt1);
        lb_bj->setObjectName(QString::fromUtf8("lb_bj"));
        lb_bj->setGeometry(QRect(220, 160, 61, 51));

        retranslateUi(Qt1);

        QMetaObject::connectSlotsByName(Qt1);
    } // setupUi

    void retranslateUi(QDialog *Qt1)
    {
        Qt1->setWindowTitle(QApplication::translate("Qt1", "Dialog", 0, QApplication::UnicodeUTF8));
        lb_time->setText(QString());
        lb_pic->setText(QString());
        label_camera->setText(QApplication::translate("Qt1", "Data Collection", 0, QApplication::UnicodeUTF8));
        pb_prev->setText(QApplication::translate("Qt1", "Prev", 0, QApplication::UnicodeUTF8));
        pb_next->setText(QApplication::translate("Qt1", "Next", 0, QApplication::UnicodeUTF8));
        pb_history->setText(QApplication::translate("Qt1", "OpenHis", 0, QApplication::UnicodeUTF8));
        pb_del->setText(QApplication::translate("Qt1", "Delete", 0, QApplication::UnicodeUTF8));
        pb_open->setText(QApplication::translate("Qt1", "Open", 0, QApplication::UnicodeUTF8));
        label_photo->setText(QApplication::translate("Qt1", "Local Operation", 0, QApplication::UnicodeUTF8));
        pb_start->setText(QApplication::translate("Qt1", "Start", 0, QApplication::UnicodeUTF8));
        text_time->setHtml(QApplication::translate("Qt1", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu Condensed'; font-size:9pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400;\">1s</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("Qt1", "1s", 0, QApplication::UnicodeUTF8)
        );
        label_R->setText(QApplication::translate("Qt1", "R", 0, QApplication::UnicodeUTF8));
        label_Rwarning->setText(QApplication::translate("Qt1", "Status", 0, QApplication::UnicodeUTF8));
        lb_num->setText(QString());
        lb_bj->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Qt1: public Ui_Qt1 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT1_H
