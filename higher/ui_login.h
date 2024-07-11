/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QPushButton *pb_login;
    QPushButton *pb_quit;
    QLabel *lb_username;
    QLabel *lb_password;
    QLabel *lb_tittle;
    QLineEdit *text_password;
    QLineEdit *text_username;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QFrame *line_4;
    QPushButton *pb_register;

    void setupUi(QWidget *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName(QString::fromUtf8("Login"));
        Login->resize(557, 395);
        pb_login = new QPushButton(Login);
        pb_login->setObjectName(QString::fromUtf8("pb_login"));
        pb_login->setGeometry(QRect(90, 210, 371, 41));
        QFont font;
        font.setFamily(QString::fromUtf8("Ubuntu Condensed"));
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        pb_login->setFont(font);
        pb_quit = new QPushButton(Login);
        pb_quit->setObjectName(QString::fromUtf8("pb_quit"));
        pb_quit->setGeometry(QRect(280, 260, 181, 41));
        pb_quit->setFont(font);
        lb_username = new QLabel(Login);
        lb_username->setObjectName(QString::fromUtf8("lb_username"));
        lb_username->setGeometry(QRect(90, 100, 151, 41));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Ubuntu Condensed"));
        font1.setPointSize(15);
        font1.setBold(true);
        font1.setWeight(75);
        lb_username->setFont(font1);
        lb_password = new QLabel(Login);
        lb_password->setObjectName(QString::fromUtf8("lb_password"));
        lb_password->setGeometry(QRect(90, 150, 171, 41));
        lb_password->setFont(font1);
        lb_tittle = new QLabel(Login);
        lb_tittle->setObjectName(QString::fromUtf8("lb_tittle"));
        lb_tittle->setGeometry(QRect(60, 50, 431, 41));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Ubuntu Condensed"));
        font2.setPointSize(18);
        font2.setBold(true);
        font2.setWeight(75);
        lb_tittle->setFont(font2);
        text_password = new QLineEdit(Login);
        text_password->setObjectName(QString::fromUtf8("text_password"));
        text_password->setGeometry(QRect(270, 150, 191, 31));
        text_username = new QLineEdit(Login);
        text_username->setObjectName(QString::fromUtf8("text_username"));
        text_username->setGeometry(QRect(270, 110, 191, 31));
        line = new QFrame(Login);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(20, 370, 521, 21));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(Login);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 20, 20, 361));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(Login);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(530, 20, 20, 361));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(Login);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(20, 10, 521, 21));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        pb_register = new QPushButton(Login);
        pb_register->setObjectName(QString::fromUtf8("pb_register"));
        pb_register->setGeometry(QRect(90, 260, 171, 41));
        pb_register->setFont(font);

        retranslateUi(Login);

        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QWidget *Login)
    {
        Login->setWindowTitle(QApplication::translate("Login", "Form", nullptr));
        pb_login->setText(QApplication::translate("Login", "Login", nullptr));
        pb_quit->setText(QApplication::translate("Login", "Quit", nullptr));
        lb_username->setText(QApplication::translate("Login", "Username", nullptr));
        lb_password->setText(QApplication::translate("Login", "Password", nullptr));
        lb_tittle->setText(QApplication::translate("Login", "Data Acquisition System", nullptr));
        pb_register->setText(QApplication::translate("Login", "Register", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
