#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    digitBTNs ={{Qt::Key_0,ui->btnNum0},
                 {Qt::Key_1,ui->btnNum1},
                 {Qt::Key_2,ui->btnNum2},
                 {Qt::Key_3, ui->btnNum3},
                 {Qt::Key_4, ui->btnNum4},
                 {Qt::Key_5, ui->btnNum5},
                 {Qt::Key_6, ui->btnNum6},
                 {Qt::Key_7, ui->btnNum7},
                 {Qt::Key_8, ui->btnNum8},
                 {Qt::Key_9, ui->btnNum9},
                 };

    foreach(auto btn, digitBTNs)
        connect(btn,SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    connect(ui->btnPlus, SIGNAL(clicked()),this, SLOT(btnBinaryOperatprClicked()));
    connect(ui->btnMinus, SIGNAL(clicked()),this, SLOT(btnBinaryOperatprClicked()));
    connect(ui->btnMultiple, SIGNAL(clicked()),this, SLOT(btnBinaryOperatprClicked()));
    connect(ui->btnDivide, SIGNAL(clicked()),this, SLOT(btnBinaryOperatprClicked()));

    connect(ui->btnPercentage,SIGNAL(clicked()),this, SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnInverse, SIGNAL(clicked()),this, SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSquare, SIGNAL(clicked()),this, SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSqrt, SIGNAL(clicked()),this, SLOT(btnUnaryOperatorClicked()));

    connect(ui->btnSign, SIGNAL(clicked()), this, SLOT(btnUnaryOperatorClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::calculation(bool *ok)
{
    double result = 0;
    if(operands.size() >= 2 && !opcodes.isEmpty()){
        double operand2 = operands.pop().toDouble();
        double operand1 = operands.pop().toDouble();
        QString op = opcodes.pop();

        if(op =="+"){
            result = operand1 + operand2;
        }else if(op =="-"){
            result = operand1 - operand2;
        }else if(op =="*"){
            result = operand1 * operand2;
        }else if(op =="/"){
            if(operand2 == 0){
                ui->statusbar->showMessage("错误：除数不能为0");
                if(ok) *ok = false;
                return "错误";
            }
            result = operand1 / operand2;
        }
        operands.push(QString::number(result));
        ui->statusbar->showMessage("计算中...");
    } else if(operands.size() == 1 && opcodes.isEmpty()){
        result = operands.top().toDouble();
    } else {
        ui->statusbar->showMessage(QString("操作数: %1, 操作符: %2").arg(operands.size()).arg(opcodes.size()));
        if(ok) *ok = false;
        return "";
    }

    if(result == (long long)result)
        return QString::number((long long)result);
    else
        return QString::number(result);
}

void MainWindow::btnNumClicked()
{
    QString digit = qobject_cast<QPushButton*>(sender())->text();
    if(digit == "0" && operand == "0")
        return;
    if(operand == "0" && digit != "0")
        operand = "";
    operand += digit;
    ui->display->setText(operand);
}

void MainWindow::on_btnPeriod_clicked()
{
    if(!operand.contains(".") && !operand.isEmpty())
        operand += ".";
    ui->display->setText(operand);
}

void MainWindow::on_btnDel_clicked()
{
    if(!operand.isEmpty()){
        operand = operand.left(operand.length() - 1);
        ui->display->setText(operand);
    }
}

void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    operands.clear();
    opcodes.clear();
    ui->display->setText("");
    ui->statusbar->clearMessage();
}

void MainWindow::btnBinaryOperatprClicked()
{
    QString currentOp = qobject_cast<QPushButton*>(sender())->text();
    if (currentOp == "×") currentOp = "*";
    else if (currentOp == "÷") currentOp = "/";

    if(!operand.isEmpty()){
        operands.push(operand);
        operand.clear();
    }

    if(!opcodes.isEmpty() && operands.size() >= 2){
        QString temp = calculation();
        if(!temp.isEmpty())
            ui->display->setText(temp);
    }

    opcodes.push(currentOp);
    ui->statusbar->showMessage("操作符: " + currentOp);
}

void MainWindow::btnUnaryOperatorClicked()
{
    if(operand.isEmpty() && operands.isEmpty())
        return;

    double result;
    if(operand.isEmpty() && !operands.isEmpty()){
        result = operands.pop().toDouble();
    } else {
        result = operand.toDouble();
        operand.clear();
    }

    QString op = qobject_cast<QPushButton*>(sender())->text();
    bool ok = true;

    if(op =="%"){
        result /= 100.0;
    }else if(op == "1/x"){
        if(result == 0){
            ui->statusbar->showMessage("错误：除数不能为0");
            return;
        }
        result = 1 / result;
    }else if(op =="x^2"){
        result *= result;
    }else if(op =="√"){
        if(result < 0) {
            ui->statusbar->showMessage("错误：负数不能开根号");
            return;
        }
        result = sqrt(result);
    }

    operand = QString::number(result);
    operands.push(operand);
    operand.clear();
    ui->display->setText(QString::number(result));
}

void MainWindow::on_btnEqual_clicked()
{
    if(operand.isEmpty() && operands.isEmpty())
        return;

    if(!operand.isEmpty()){
        operands.push(operand);
        operand.clear();
    }

    while(operands.size() >= 2 && !opcodes.isEmpty()){
        QString result = calculation();
        if(result == "错误")
            return;
        ui->display->setText(result);
    }

    if(!operands.isEmpty()){
        operand = operands.pop();
        operands.push(operand);
    }
}


void MainWindow::on_btnSign_clicked()
{
    if (operand.isEmpty()) {
        operand = "-";
    } else if (operand == "-") {
        operand = "";
    } else {
        if (operand.startsWith("-")) {
            operand.remove(0, 1);
        } else {
            operand = "-" + operand;
        }
    }
    ui->display->setText(operand);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    foreach(auto btnkey, digitBTNs.keys()){
        if(event->key() == btnkey){
            digitBTNs[btnkey]->animateClick();
            return;
        }
    }

    switch(event->key()){
    case Qt::Key_Plus: ui->btnPlus->animateClick(); break;
    case Qt::Key_Minus:

        if (event->modifiers() & Qt::ShiftModifier) {
            ui->btnSign->animateClick();
        } else {
            ui->btnMinus->animateClick();
        }
        break;
    case Qt::Key_Asterisk: ui->btnMultiple->animateClick(); break;
    case Qt::Key_Slash: ui->btnDivide->animateClick(); break;
    case Qt::Key_Enter:
    case Qt::Key_Return: ui->btnEqual->animateClick(); break;
    case Qt::Key_Period: ui->btnPeriod->animateClick(); break;
    case Qt::Key_Backspace: ui->btnDel->animateClick(); break;
    case Qt::Key_Escape: ui->btnClear->animateClick(); break;
    }
}
