#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    textChanged = false;
    on_actionNew_triggered();

    statusLabel.setMaximumWidth(150);
    statusLabel.setText("length:"+QString::number(0)+"     lines:"+ QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);


    statusCursorLabel.setMaximumWidth(150);
    statusCursorLabel.setText("Ln:"+QString::number(0)+"     Col:"+ QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText(tr("郑宇"));
    ui->statusbar->addPermanentWidget(author);

    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionPaste->setEnabled(false);

    QPlainTextEdit::LineWrapMode mode =ui->TextEdit->lineWrapMode();

    if(mode == QTextEdit::NoWrap ){
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionWrap->setChecked(false);
    } else {ui->TextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actionWrap->setChecked(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()//关于窗口
{
    AboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()//查找窗口
{
    SearchDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()//替换窗口
{
    ReplaceDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()//新建
{
    if(!userEditConfirmed())
        return;
    filePath="";

    ui->TextEdit->clear();
    this->setWindowTitle(tr("新建文本文件 -编辑器"));

    textChanged = false;
}


void MainWindow::on_actionOpen_triggered()//打开
{
    if(!userEditConfirmed())
        return;
QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text files (*.txt);; A11 (*.*)"));
QFile file(filename);

if(!file.open(QFile::ReadOnly|QFile::Text)){
QMessageBox::warning(this,"..","打开文件失败");
    return;
}

filePath =filename;

QTextStream in(&file);
QString text = in.readAll();
ui->TextEdit->insertPlainText(text);
file.close();

this->setWindowTitle(QFileInfo(filename).absoluteFilePath());

textChanged = false;

}


void MainWindow::on_actionSave_triggered()//保存
{
    if(filePath == ""){
        QString filename =QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text files(*.txt)"));

        QFile file(filename);
        if(!file.open(QFile::WriteOnly |QFile::Text)){
            QMessageBox::warning(this,"..","打开保存文件失败");
            return;
        }
        file.close();
        filePath = filename;
    }

QFile file(filePath);

if(!file.open(QFile::WriteOnly|QFile::Text)){
    QMessageBox::warning(this,"..","打开文件失败");

    QString filename =QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text files(*.txt)"));

    QFile file(filename);
    if(!file.open(QFile::WriteOnly |QFile::Text)){
    QMessageBox::warning(this,"..","打开保存文件失败");
    return;
    }

}
QTextStream out(&file);
QString text =ui->TextEdit->toPlainText();
out << text;
file.flush();
file.close();

this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

textChanged =false;
}


void MainWindow::on_actionSaveAs_triggered()//另存为
{
    QString filename =QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text files(*.txt)"));

    QFile file(filename);
    if(!file.open(QFile::WriteOnly |QFile::Text)){
        QMessageBox::warning(this,"..","打开保存文件失败");
        return;
    }
    filePath = filename;
    QTextStream out(&file);
    QString text =ui->TextEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}


void MainWindow::on_TextEdit_textChanged()//文本变化
{
    if(!textChanged){
        this->setWindowTitle("*"+ this->windowTitle());
        textChanged = true;
    }
}

bool MainWindow::userEditConfirmed()
{
    QString path=(filePath!="")?filePath :"无标题.txt";

    if(textChanged){
        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n")+"\""+ path +"\"?");
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        int r=msg.exec();
        switch(r){
        case QMessageBox::Yes:
            on_actionSave_triggered();
            break;
        case QMessageBox::No:
            textChanged =false;
            break;
        case QMessageBox::Cancel:
            return false;
        }
    }

}

void MainWindow::on_actionUndo_triggered()//撤销
{
    ui->TextEdit->undo();
}


void MainWindow::on_actionRedo_triggered()//恢复
{
    ui->TextEdit->redo();
}


void MainWindow::on_actionCut_triggered()//剪切
{
    ui->TextEdit->cut();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionCopy_triggered()//复制
{
    ui->TextEdit->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()//粘贴
{
    ui->TextEdit->paste();
}


void MainWindow::on_TextEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}


void MainWindow::on_TextEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}


void MainWindow::on_TextEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}


void MainWindow::on_actionFontColor_triggered()//字体颜色
{
    QColor color=QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid()){
        ui->TextEdit->setStyleSheet(QString("QPlainTextEdit {color:%1}").arg(color.name()));
    }
}


void MainWindow::on_actionBColor_triggered()//背景颜色
{
    QColor color=QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid()){
        ui->TextEdit->setStyleSheet(QString("QPlainTextEdit {background-color: %1}").arg(color.name()));
    }
}


void MainWindow::on_actionColor_triggered()//字体背景色
{

}


void MainWindow::on_actionWrap_triggered()//自动换行
{
    QPlainTextEdit::LineWrapMode mode =ui->TextEdit->lineWrapMode();
    if(mode == QTextEdit::NoWrap ){
        ui->TextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionWrap->setChecked(true);
    } else {ui->TextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actionWrap->setChecked(false);
    }
}


void MainWindow::on_actionFont_triggered()//字体
{
    bool ok=false;
    QFont font = QFontDialog::getFont(&ok, this);

    if(ok)
        ui->TextEdit->setFont(font);
}

