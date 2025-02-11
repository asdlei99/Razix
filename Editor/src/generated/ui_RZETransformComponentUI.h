/********************************************************************************
** Form generated from reading UI file 'RZETransformComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZETRANSFORMCOMPONENTUI_H
#define UI_RZETRANSFORMCOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TransformComponent
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *PositionLayout;
    QLabel *PosLbl;
    QSpacerItem *horizontalSpacer_1;
    QPushButton *PosX;
    QLineEdit *PosVal_X;
    QPushButton *PosY;
    QLineEdit *PosVal_Y;
    QPushButton *PosZ;
    QLineEdit *PosVal_Z;
    QHBoxLayout *RotationLayout;
    QLabel *RotLbl;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *RotX;
    QLineEdit *RotVal_X;
    QPushButton *RotY;
    QLineEdit *RotVal_Y;
    QPushButton *RotZ;
    QLineEdit *RotVal_Z;
    QHBoxLayout *ScaleLayout;
    QLabel *ScaleLbl;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *ScaleX;
    QLineEdit *ScaleVal_X;
    QPushButton *ScaleY;
    QLineEdit *ScaleVal_Y;
    QPushButton *ScaleZ;
    QLineEdit *ScaleVal_Z;

    void setupUi(QWidget *TransformComponent)
    {
        if (TransformComponent->objectName().isEmpty())
            TransformComponent->setObjectName(QString::fromUtf8("TransformComponent"));
        TransformComponent->resize(350, 88);
        TransformComponent->setMinimumSize(QSize(350, 0));
        TransformComponent->setStyleSheet(QString::fromUtf8("border-left: 2px;\n"
"border-right: 2px;\n"
"border-bottom: 2px;\n"
"\n"
""));
        verticalLayout = new QVBoxLayout(TransformComponent);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        PositionLayout = new QHBoxLayout();
        PositionLayout->setObjectName(QString::fromUtf8("PositionLayout"));
        PosLbl = new QLabel(TransformComponent);
        PosLbl->setObjectName(QString::fromUtf8("PosLbl"));
        PosLbl->setMinimumSize(QSize(50, 0));

        PositionLayout->addWidget(PosLbl);

        horizontalSpacer_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        PositionLayout->addItem(horizontalSpacer_1);

        PosX = new QPushButton(TransformComponent);
        PosX->setObjectName(QString::fromUtf8("PosX"));
        PosX->setMaximumSize(QSize(20, 20));
        PosX->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));

        PositionLayout->addWidget(PosX);

        PosVal_X = new QLineEdit(TransformComponent);
        PosVal_X->setObjectName(QString::fromUtf8("PosVal_X"));
        PosVal_X->setMinimumSize(QSize(60, 0));
        PosVal_X->setMaximumSize(QSize(80, 20));
        PosVal_X->setDragEnabled(true);

        PositionLayout->addWidget(PosVal_X);

        PosY = new QPushButton(TransformComponent);
        PosY->setObjectName(QString::fromUtf8("PosY"));
        PosY->setMaximumSize(QSize(20, 20));
        PosY->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));

        PositionLayout->addWidget(PosY);

        PosVal_Y = new QLineEdit(TransformComponent);
        PosVal_Y->setObjectName(QString::fromUtf8("PosVal_Y"));
        PosVal_Y->setMinimumSize(QSize(60, 0));
        PosVal_Y->setMaximumSize(QSize(80, 20));

        PositionLayout->addWidget(PosVal_Y);

        PosZ = new QPushButton(TransformComponent);
        PosZ->setObjectName(QString::fromUtf8("PosZ"));
        PosZ->setMaximumSize(QSize(20, 20));
        PosZ->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));

        PositionLayout->addWidget(PosZ);

        PosVal_Z = new QLineEdit(TransformComponent);
        PosVal_Z->setObjectName(QString::fromUtf8("PosVal_Z"));
        PosVal_Z->setMinimumSize(QSize(60, 0));
        PosVal_Z->setMaximumSize(QSize(80, 20));

        PositionLayout->addWidget(PosVal_Z);


        verticalLayout->addLayout(PositionLayout);

        RotationLayout = new QHBoxLayout();
        RotationLayout->setObjectName(QString::fromUtf8("RotationLayout"));
        RotLbl = new QLabel(TransformComponent);
        RotLbl->setObjectName(QString::fromUtf8("RotLbl"));
        RotLbl->setMinimumSize(QSize(50, 0));

        RotationLayout->addWidget(RotLbl);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        RotationLayout->addItem(horizontalSpacer_2);

        RotX = new QPushButton(TransformComponent);
        RotX->setObjectName(QString::fromUtf8("RotX"));
        RotX->setMaximumSize(QSize(20, 20));
        RotX->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));

        RotationLayout->addWidget(RotX);

        RotVal_X = new QLineEdit(TransformComponent);
        RotVal_X->setObjectName(QString::fromUtf8("RotVal_X"));
        RotVal_X->setMinimumSize(QSize(60, 0));
        RotVal_X->setMaximumSize(QSize(80, 20));
        RotVal_X->setDragEnabled(true);

        RotationLayout->addWidget(RotVal_X);

        RotY = new QPushButton(TransformComponent);
        RotY->setObjectName(QString::fromUtf8("RotY"));
        RotY->setMaximumSize(QSize(20, 20));
        RotY->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));

        RotationLayout->addWidget(RotY);

        RotVal_Y = new QLineEdit(TransformComponent);
        RotVal_Y->setObjectName(QString::fromUtf8("RotVal_Y"));
        RotVal_Y->setMinimumSize(QSize(60, 0));
        RotVal_Y->setMaximumSize(QSize(80, 20));

        RotationLayout->addWidget(RotVal_Y);

        RotZ = new QPushButton(TransformComponent);
        RotZ->setObjectName(QString::fromUtf8("RotZ"));
        RotZ->setMaximumSize(QSize(20, 20));
        RotZ->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));

        RotationLayout->addWidget(RotZ);

        RotVal_Z = new QLineEdit(TransformComponent);
        RotVal_Z->setObjectName(QString::fromUtf8("RotVal_Z"));
        RotVal_Z->setMinimumSize(QSize(60, 0));
        RotVal_Z->setMaximumSize(QSize(80, 20));

        RotationLayout->addWidget(RotVal_Z);


        verticalLayout->addLayout(RotationLayout);

        ScaleLayout = new QHBoxLayout();
        ScaleLayout->setObjectName(QString::fromUtf8("ScaleLayout"));
        ScaleLbl = new QLabel(TransformComponent);
        ScaleLbl->setObjectName(QString::fromUtf8("ScaleLbl"));
        ScaleLbl->setMinimumSize(QSize(50, 0));

        ScaleLayout->addWidget(ScaleLbl);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        ScaleLayout->addItem(horizontalSpacer_3);

        ScaleX = new QPushButton(TransformComponent);
        ScaleX->setObjectName(QString::fromUtf8("ScaleX"));
        ScaleX->setMaximumSize(QSize(20, 20));
        ScaleX->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));

        ScaleLayout->addWidget(ScaleX);

        ScaleVal_X = new QLineEdit(TransformComponent);
        ScaleVal_X->setObjectName(QString::fromUtf8("ScaleVal_X"));
        ScaleVal_X->setMinimumSize(QSize(60, 0));
        ScaleVal_X->setMaximumSize(QSize(80, 20));
        ScaleVal_X->setDragEnabled(true);

        ScaleLayout->addWidget(ScaleVal_X);

        ScaleY = new QPushButton(TransformComponent);
        ScaleY->setObjectName(QString::fromUtf8("ScaleY"));
        ScaleY->setMaximumSize(QSize(20, 20));
        ScaleY->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));

        ScaleLayout->addWidget(ScaleY);

        ScaleVal_Y = new QLineEdit(TransformComponent);
        ScaleVal_Y->setObjectName(QString::fromUtf8("ScaleVal_Y"));
        ScaleVal_Y->setMinimumSize(QSize(60, 0));
        ScaleVal_Y->setMaximumSize(QSize(80, 20));

        ScaleLayout->addWidget(ScaleVal_Y);

        ScaleZ = new QPushButton(TransformComponent);
        ScaleZ->setObjectName(QString::fromUtf8("ScaleZ"));
        ScaleZ->setMaximumSize(QSize(20, 20));
        ScaleZ->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));

        ScaleLayout->addWidget(ScaleZ);

        ScaleVal_Z = new QLineEdit(TransformComponent);
        ScaleVal_Z->setObjectName(QString::fromUtf8("ScaleVal_Z"));
        ScaleVal_Z->setMinimumSize(QSize(60, 0));
        ScaleVal_Z->setMaximumSize(QSize(80, 20));

        ScaleLayout->addWidget(ScaleVal_Z);


        verticalLayout->addLayout(ScaleLayout);


        retranslateUi(TransformComponent);

        QMetaObject::connectSlotsByName(TransformComponent);
    } // setupUi

    void retranslateUi(QWidget *TransformComponent)
    {
        TransformComponent->setWindowTitle(QCoreApplication::translate("TransformComponent", "Form", nullptr));
        PosLbl->setText(QCoreApplication::translate("TransformComponent", "Position", nullptr));
        PosX->setText(QCoreApplication::translate("TransformComponent", "x", nullptr));
        PosVal_X->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        PosY->setText(QCoreApplication::translate("TransformComponent", "y", nullptr));
        PosVal_Y->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        PosZ->setText(QCoreApplication::translate("TransformComponent", "z", nullptr));
        PosVal_Z->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        RotLbl->setText(QCoreApplication::translate("TransformComponent", "Rotation", nullptr));
        RotX->setText(QCoreApplication::translate("TransformComponent", "x", nullptr));
        RotVal_X->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        RotY->setText(QCoreApplication::translate("TransformComponent", "y", nullptr));
        RotVal_Y->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        RotZ->setText(QCoreApplication::translate("TransformComponent", "z", nullptr));
        RotVal_Z->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        ScaleLbl->setText(QCoreApplication::translate("TransformComponent", "Scale", nullptr));
        ScaleX->setText(QCoreApplication::translate("TransformComponent", "x", nullptr));
        ScaleVal_X->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        ScaleY->setText(QCoreApplication::translate("TransformComponent", "y", nullptr));
        ScaleVal_Y->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        ScaleZ->setText(QCoreApplication::translate("TransformComponent", "z", nullptr));
        ScaleVal_Z->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TransformComponent: public Ui_TransformComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZETRANSFORMCOMPONENTUI_H
