#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database=QSqlDatabase::addDatabase("QSQLITE");//添加 SQL LITE数据库驱动
    QString aFile="E:/Qt/qt/实验3/link/Lab4a.db";
    database.setDatabaseName(aFile);//设置数据库名称

    if(!database.open()){//打开数据库
        qDebug() << "failed to open database";
    } else
        qDebug() << "open database is ok"<< database.connectionName();;
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(
        QSqlTableModel::OnManualSubmit);//数据保存方式, OnManualSubmit,OnRowChange
    patientTabModel->setSort(patientTabModel->fieldIndex("name"), Qt::AscendingOrder);//排序
    if(!(patientTabModel->select()))//查询数据
        return false;
    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),
                               QModelIndex());//在末尾添加一个记录
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1,
                                                1);//创建最后一行的ModelIndex
    int curRecNo = curIndex.row();
    QSqlRecord curRec =patientTabModel->record(curRecNo);//获取当前记录
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));
    patientTabModel->setRecord(curRecNo, curRec);

    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex= thePatientSelection->currentIndex();//获取当前选择单元格的模型索引
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString password)
{
        //return "loginOK";
    QSqlQuery query;//查询出当前记录的所有字段
    query.prepare("select username,password from user where username = :USER");
    query.bindValue(":USER",userName);
    query.exec();
    if (query.first() && query.value("username").isValid()) {
        QString passwd = query.value("password").toString();
        if (passwd == password) {
             qDebug() << "login ok";
            return "loginOk";
        }else {
             qDebug() << "wrong Password";
            return "wrongPassword";
        }
    } else {
            qDebug() << "no such user";
            return "wrongUsername";
        }
}

IDatabase::IDatabase(QObject *parent): QObject{parent}
{
    ininDatabase();
}
