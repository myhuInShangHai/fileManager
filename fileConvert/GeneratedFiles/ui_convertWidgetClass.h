/********************************************************************************
** Form generated from reading UI file 'convertWidgetClass.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONVERTWIDGETCLASS_H
#define UI_CONVERTWIDGETCLASS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_convertWidgetClass
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_3;
    QComboBox *typeSelect;
    QLabel *label_3;
    QLineEdit *fileName;
    QListWidget *languageList;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QLabel *label_5;
    QFrame *line_2;
    QFrame *line;
    QPushButton *okButton;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *sourceFile;
    QLineEdit *destDir;

    void setupUi(QWidget *convertWidgetClass)
    {
        if (convertWidgetClass->objectName().isEmpty())
            convertWidgetClass->setObjectName(QStringLiteral("convertWidgetClass"));
        convertWidgetClass->resize(413, 486);
        verticalLayout = new QVBoxLayout(convertWidgetClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(convertWidgetClass);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        label_6 = new QLabel(frame);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);

        gridLayout->addWidget(label_6, 7, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(291, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 8, 0, 1, 4);

        typeSelect = new QComboBox(frame);
        typeSelect->setObjectName(QStringLiteral("typeSelect"));

        gridLayout->addWidget(typeSelect, 0, 2, 1, 3);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        fileName = new QLineEdit(frame);
        fileName->setObjectName(QStringLiteral("fileName"));

        gridLayout->addWidget(fileName, 5, 2, 1, 1);

        languageList = new QListWidget(frame);
        languageList->setObjectName(QStringLiteral("languageList"));
        languageList->setBatchSize(100);

        gridLayout->addWidget(languageList, 7, 2, 1, 3);

        horizontalSpacer_2 = new QSpacerItem(106, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 5, 3, 1, 2);

        horizontalSpacer = new QSpacerItem(294, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 2, 1, 3);

        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        line_2 = new QFrame(frame);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_2, 6, 0, 1, 5);

        line = new QFrame(frame);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 5);

        okButton = new QPushButton(frame);
        okButton->setObjectName(QStringLiteral("okButton"));

        gridLayout->addWidget(okButton, 8, 4, 1, 1);

        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        sourceFile = new QLineEdit(frame);
        sourceFile->setObjectName(QStringLiteral("sourceFile"));

        gridLayout->addWidget(sourceFile, 3, 2, 1, 3);

        destDir = new QLineEdit(frame);
        destDir->setObjectName(QStringLiteral("destDir"));

        gridLayout->addWidget(destDir, 4, 2, 1, 3);


        verticalLayout->addWidget(frame);


        retranslateUi(convertWidgetClass);

        languageList->setCurrentRow(-1);


        QMetaObject::connectSlotsByName(convertWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *convertWidgetClass)
    {
        convertWidgetClass->setWindowTitle(QApplication::translate("convertWidgetClass", "\346\226\207\344\273\266\350\275\254\345\214\226", 0));
        label_4->setText(QApplication::translate("convertWidgetClass", "\350\276\223\345\207\272\347\233\256\345\275\225\357\274\232", 0));
        label_6->setText(QApplication::translate("convertWidgetClass", "\350\257\255\350\250\200\351\200\211\346\213\251\357\274\232", 0));
        label_3->setText(QApplication::translate("convertWidgetClass", "\351\200\211\346\213\251\346\272\220\346\226\207\344\273\266\357\274\232", 0));
        label_5->setText(QApplication::translate("convertWidgetClass", "\346\226\207\344\273\266\345\220\215\357\274\232", 0));
        okButton->setText(QApplication::translate("convertWidgetClass", "\347\241\256\345\256\232", 0));
        label->setText(QApplication::translate("convertWidgetClass", "\345\267\245\345\205\267\347\261\273\345\236\213\357\274\232", 0));
        label_2->setText(QApplication::translate("convertWidgetClass", "\346\226\207\344\273\266\350\256\276\347\275\256", 0));
    } // retranslateUi

};

namespace Ui {
    class convertWidgetClass: public Ui_convertWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONVERTWIDGETCLASS_H
