#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textChanged = false;

    on_actNew_triggered();

    statusLabel.setMaximumWidth(180);
    statusLabel.setText("length: " + QString::number(0) + "    lines: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(180);
    statusCursorLabel.setText("ln: " + QString::number(0) + "    col: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel("ui->statusbar");
    author->setText(tr("郑宇"));
    ui->statusbar->addPermanentWidget(author);

    ui->actCopy->setEnabled(false);
    ui->actCut->setEnabled(false);
    ui->actUndo->setEnabled(false);
    ui->actRedo->setEnabled(false);
    ui->actPaste->setEnabled(false);

    QPlainTextEdit::LineWrapMode mode = ui->textEdit->lineWrapMode();

    if(mode == QTextEdit::NoWrap) {
        ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        ui->actWrap->setChecked(false);
    }else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actWrap->setChecked(true);
    }

    ui->actToolBar->setChecked(true);
    ui->actStatusBar->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actAbout_triggered()
{
    AboutDialog dig;
    dig.exec();
}


void MainWindow::on_actFind_triggered()
{
    FindDialog dig(this, ui->textEdit);
    dig.exec();
}


void MainWindow::on_actReplace_triggered()
{
    ReplaceDialog dig(this, ui->textEdit);
    dig.exec();
}


void MainWindow::on_actNew_triggered()
{
    if(!userEditConfirmed())
        return;

    filePath = "";
    ui->textEdit->clear();
    this->setWindowTitle(tr("新建文本文件--编辑器"));

    textChanged = false;
}


void MainWindow::on_actOpen_triggered()
{
    if(!userEditConfirmed())
        return;
    QString filename = QFileDialog::getOpenFileName(this, "打开文件", ".", tr("Text files (*.txt);; All(*.*)"));
    QFile file(filename);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "..", "打开文件失败");
        return;
    }
    filePath = filename;

    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->clear();
    ui->textEdit->insertPlainText(text);
    file.close();

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());

    textChanged = false;
}


void MainWindow::on_actSave_triggered()
{
    if(filePath == ""){
        QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".",
                                                        tr("Text files (*.txt)"));
        QFile file(filename);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "警告", "保存文件失败");
            return;
        }
        file.close();
        filePath = filename; // 更新路径
    }
    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "保存文件失败");
        return;
    }
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged = false;
}


void MainWindow::on_actSaveAs_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".",
                                                    tr("Text files (*.txt)"));
    QFile file(filename);
    if (filename.isEmpty())
        return; // 用户取消了保存

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "保存文件失败");
        return;
    }

    filePath = filename;
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged = false;
}


void MainWindow::on_textEdit_textChanged()
{
    if(!textChanged){
        this->setWindowTitle("*" + this->windowTitle());
        textChanged = true;
    }

    statusLabel.setText("length: " + QString::number(ui->textEdit->toPlainText().length()) +
                        "    lines: " + QString::number(ui->textEdit->document()->lineCount()));

}

bool MainWindow::userEditConfirmed()
{
    if(textChanged){

        QString path = (filePath != "") ? filePath : "无标题.txt";

        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setText(QString("是否将更改保存到\n") + "\"" + path + "\" ?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int r = msg.exec();
        switch(r){
        case QMessageBox::Yes:
            on_actSave_triggered();
            break;
        case QMessageBox::No:
            textChanged = false;
            break;
        case QMessageBox::Cancel:
            return false;
        }
    }
    return true;
}



void MainWindow::on_actUndo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actCut_triggered()
{
    ui->textEdit->cut();
    ui->actPaste->setEnabled(true);
}


void MainWindow::on_actCopy_triggered()
{
    ui->textEdit->copy();
    ui->actPaste->setEnabled(true);
}


void MainWindow::on_actPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actRedo_triggered()
{
    ui->textEdit->redo();
}


void MainWindow::on_textEdit_undoAvailable(bool b)
{
    ui->actUndo->setEnabled(b);
}


void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->actCopy->setEnabled(b);
    ui->actCut->setEnabled(b);
}


void MainWindow::on_textEdit_redoAvailable(bool b)
{
    ui->actRedo->setEnabled(b);
}


void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if(color.isValid()){
        ui->textEdit->setStyleSheet(QString("QPlainTextEdit {color: %1}").arg(color.name()));
    }
}


void MainWindow::on_actEditorBackgroundColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if(color.isValid()){
        ui->textEdit->setStyleSheet(QString("QPlainTextEdit {background-color: %1}").arg(color.name()));
    }
}


void MainWindow::on_actWrap_triggered()
{
    QPlainTextEdit::LineWrapMode mode = ui->textEdit->lineWrapMode();

    if(mode == QTextEdit::NoWrap) {
        ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        ui->actWrap->setChecked(true);
    }else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actWrap->setChecked(false);
    }
}


void MainWindow::on_actFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, this);

    if(ok){
        ui->textEdit->setFont(font);
    }
}


void MainWindow::on_actStatusBar_triggered()
{
    bool visible = ui->statusbar->isVisible();
    ui->statusbar->setVisible(!visible);
    ui->actStatusBar->setChecked(!visible);
}


void MainWindow::on_actToolBar_triggered()
{
    bool visible = ui->toolBar->isVisible();
    ui->toolBar->setVisible(!visible);
    ui->actToolBar->setChecked(!visible);
}


void MainWindow::on_actSelectAll_triggered()
{
    ui->textEdit->selectAll();
}


void MainWindow::on_actExit_triggered()
{
    if(userEditConfirmed()){
        exit(0);
    }
}


void MainWindow::on_textEdit_cursorPositionChanged()
{
    int col = 0;
    int ln = 0;
    int flg = -1;
    int pos = ui->textEdit->textCursor().position();
    QString text = ui->textEdit->toPlainText();

    for(int i = 0; i < pos; i++){
        if(text[i] == '\n'){
            ln++;
            flg = i;
        }
    }
    flg++;
    col = pos-flg;

    statusCursorLabel.setText("ln: " + QString::number(ln + 1) + "    col: " + QString::number(col + 1));
}

