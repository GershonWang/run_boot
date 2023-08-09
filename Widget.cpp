#include "Widget.h"
#include "ui_widget.h"
/**
 * 初始化页面数据
 * @brief Widget::Widget
 * @param parent
 */
Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget){
    // 设置UI界面布局
    ui->setupUi(this);
    // Qt提供的列表文本模型
    model = new QStringListModel(this);
    ui->list_serv->setModel(model);
    // 初始化默认目录以及启动服务列表
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation); // home目录
    dstFileName = homeDir + "/.run-boot"; // 默认服务存放位置
    QDir directory(dstFileName);
    if (!directory.exists()) {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        ui->text_log->append(tr("「%1」目录不存在。。。").arg(formattedDateTime));
        return;
    }
    QStringList fileList = directory.entryList(QDir::Files);
    foreach (const QString &file, fileList) {
        serverList.append(file);
    }
    model->setStringList(serverList);
}

Widget::~Widget()
{
    delete ui;
}
/**
 * 上传按钮
 * @brief Widget::on_btn_upload_clicked
 */
void Widget::on_btn_upload_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择一个启动服务", "", "启动服务 (*.jar *.py)");
    QFile *localFile = new QFile(fileName);
    localFile->open(QFile::ReadOnly);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    if(!fileName.isEmpty()) {
        ui->text_log->append(tr("「%1」已打开文件 %2").arg(formattedDateTime).arg(fileName));
        QStringList list = fileName.split("/");
        serverName = list.at(list.size() - 1);
        QString fullPath = dstFileName + "/" + serverName;
        QDir dir(dstFileName);
        if(!dir.exists()){
           bool ok = dir.mkpath(dstFileName); //创建多级目录
           if(ok) {
               ui->text_log->append(tr("「%1」目录不存在，创建目录成功").arg(formattedDateTime));
           } else {
               ui->text_log->append(tr("「%1」创建目录失败，没有权限").arg(formattedDateTime));
           }
        }
        localFile->copy(fullPath);
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        if(!serverList.contains(serverName)) {
            ui->text_log->append(tr("「%1」文件存放在 %2").arg(formattedDateTime).arg(fullPath));
            serverList.append(serverName); // 将服务名称存放模型集合中
            model->setStringList(serverList); // 只要已经将模型和视图绑定，那么模型中的数据就可以在视图中显示
        } else {
            ui->text_log->append(tr("「%1」文件已存在。。。").arg(formattedDateTime));
        }
    } else {
        ui->text_log->append(tr("「%1」取消选择").arg(formattedDateTime));
    }
}
/**
 * 移除服务
 * @brief Widget::on_btn_remove_clicked
 */
void Widget::on_btn_remove_clicked() {
    QItemSelectionModel *itemModel = ui->list_serv->selectionModel(); //继承自QAbstractItemModel类方法
    QStringList selectedItems; // 创建字符串数组用来存储选择的item
    QModelIndexList selectedIndexes = itemModel->selectedIndexes(); // 获取选择索引
    foreach (const QModelIndex &index, selectedIndexes) {
        QVariant data = index.data(Qt::DisplayRole);
        if (data.isValid()) {
            selectedItems << data.toString();
        }
    }
    QString selectServerName = selectedItems.at(0);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    if(!selectedItems.isEmpty() && selectServerName != NULL) {
        QString filePath = dstFileName + "/" + selectServerName;
        QFile file(filePath);
        if (file.remove()) {
            serverList.removeOne(selectServerName);
            model->setStringList(serverList);
            ui->text_log->append(tr("「%1」删除服务 %2 成功。。。").arg(formattedDateTime).arg(selectServerName));
        } else {
            serverList.removeOne(selectServerName);
            model->setStringList(serverList);
            ui->text_log->append(tr("「%1」删除服务 %2 失败。。。").arg(formattedDateTime).arg(selectServerName));
        }
    } else {
        ui->text_log->append(tr("「%1」请选择服务后，再操作。。。").arg(formattedDateTime));
    }
}
/**
 * 启动服务按钮
 * @brief Widget::on_btn_start_clicked
 */
void Widget::on_btn_start_clicked() {
    QItemSelectionModel *itemModel = ui->list_serv->selectionModel(); //继承自QAbstractItemModel类方法
    QStringList selectedItems; // 创建字符串数组用来存储选择的item
    QModelIndexList selectedIndexes = itemModel->selectedIndexes(); // 获取选择索引
    foreach (const QModelIndex &index, selectedIndexes) {
        QVariant data = index.data(Qt::DisplayRole);
        if (data.isValid()) {
            selectedItems << data.toString();
        }
    }
    QString selectServerName = selectedItems.at(0);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    if(!selectedItems.isEmpty() && selectServerName != NULL) {
        QString jarPath = dstFileName + "/" + selectServerName;
        QString command = "java"; // 要执行的命令，可以根据你的需要修改
        QStringList arguments; // 命令的参数，如果有的话
        arguments << "-jar" << jarPath;
        QProcess *process = new QProcess;
        process->setProcessChannelMode(QProcess::MergedChannels); // 合并标准输出和标准错误输出
        QObject::connect(process, &QProcess::readyReadStandardOutput, [=]() {
            QByteArray output = process->readAllStandardOutput();
            QString outputString = QString::fromLocal8Bit(output);
            ui->text_log->append(outputString);
        });
        QObject::connect(process, &QProcess::readyReadStandardError, [=]() {
            QByteArray errorOutput = process->readAllStandardError();
            QString outputString = QString::fromLocal8Bit(errorOutput);
            ui->text_log->append(outputString);
        });
        // 启动命令管道
        process->start(command,arguments);
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        // 判断是否启动
        if (!process->waitForStarted()) {
            ui->text_log->append(tr("「%1」服务 %2 启动失败").arg(formattedDateTime).arg(selectServerName));
            return;
        }
        qProcessMap.insert(selectServerName,process);
        ui->text_log->append(tr("「%1」开始启动服务： %2").arg(formattedDateTime).arg(selectServerName));
    } else {
        ui->text_log->append(tr("「%1」请选择服务后，再启动程序。。。").arg(formattedDateTime));
    }
}
/**
 * 重启服务按钮
 * @brief Widget::on_btn_restart_clicked
 */
void Widget::on_btn_restart_clicked() {
    QItemSelectionModel *itemModel = ui->list_serv->selectionModel(); //继承自QAbstractItemModel类方法
    QStringList selectedItems; // 创建字符串数组用来存储选择的item
    QModelIndexList selectedIndexes = itemModel->selectedIndexes(); // 获取选择索引
    foreach (const QModelIndex &index, selectedIndexes) {
        QVariant data = index.data(Qt::DisplayRole);
        if (data.isValid()) {
            selectedItems << data.toString();
        }
    }
    QString selectServerName = selectedItems.at(0);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    if(!selectedItems.isEmpty() && selectServerName != NULL) {
        QProcess *process = qProcessMap[selectServerName];
        if (process && process->state() == QProcess::Running) {
            process->terminate();
            ui->text_log->append(tr("「%1」停止服务 %2 成功。。。").arg(formattedDateTime).arg(selectServerName));
            QString jarPath = dstFileName + "/" + selectServerName;
            QString command = "java"; // 要执行的命令，可以根据你的需要修改
            QStringList arguments; // 命令的参数，如果有的话
            arguments << "-jar" << jarPath;
            QProcess *process = new QProcess;
            process->setProcessChannelMode(QProcess::MergedChannels); // 合并标准输出和标准错误输出
            QObject::connect(process, &QProcess::readyReadStandardOutput, [=]() {
                QByteArray output = process->readAllStandardOutput();
                QString outputString = QString::fromLocal8Bit(output);
                ui->text_log->append(outputString);
            });
            QObject::connect(process, &QProcess::readyReadStandardError, [=]() {
                QByteArray errorOutput = process->readAllStandardError();
                QString outputString = QString::fromLocal8Bit(errorOutput);
                ui->text_log->append(outputString);
            });
            // 启动命令管道
            process->start(command,arguments);
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
            // 判断是否启动
            if (!process->waitForStarted()) {
                ui->text_log->append(tr("「%1」服务 %2 启动失败").arg(formattedDateTime).arg(selectServerName));
                return;
            }
            qProcessMap.insert(selectServerName,process);
            ui->text_log->append(tr("「%1」重新启动服务： %2").arg(formattedDateTime).arg(selectServerName));
        } else {
            ui->text_log->append(tr("「%1」当前服务 %2 未启动。。。").arg(formattedDateTime).arg(selectServerName));
        }
    } else {
        ui->text_log->append(tr("「%1」请选择服务后，再重启程序。。。").arg(formattedDateTime));
    }
}
/**
 * 停止服务按钮
 * @brief Widget::on_btn_stop_clicked
 */
void Widget::on_btn_stop_clicked() {
    QItemSelectionModel *itemModel = ui->list_serv->selectionModel(); //继承自QAbstractItemModel类方法
    QStringList selectedItems; // 创建字符串数组用来存储选择的item
    QModelIndexList selectedIndexes = itemModel->selectedIndexes(); // 获取选择索引
    foreach (const QModelIndex &index, selectedIndexes) {
        QVariant data = index.data(Qt::DisplayRole);
        if (data.isValid()) {
            selectedItems << data.toString();
        }
    }
    QString selectServerName = selectedItems.at(0);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    if(!selectedItems.isEmpty() && selectedItems.at(0) != NULL) {
        QProcess *process = qProcessMap[selectServerName];
        if (process && process->state() == QProcess::Running) {
            process->terminate();
            qProcessMap.remove(selectServerName);
            process->deleteLater();
            ui->text_log->append(tr("「%1」停止服务 %2 成功。。。").arg(formattedDateTime).arg(selectServerName));
        } else {
            ui->text_log->append(tr("「%1」当前服务 %2 未启动。。。").arg(formattedDateTime).arg(selectServerName));
        }
    } else {
        ui->text_log->append(tr("「%1」请选择服务后，再停止程序。。。").arg(formattedDateTime));
    }
}
/**
 * 关于程序
 * @brief Widget::on_btn_about_clicked
 */
void Widget::on_btn_about_clicked() {
    QMessageBox::aboutQt(this,"关于QT");
}
/**
 * 监听关闭事件
 * @brief Widget::closeEvent
 * @param event
 */
void Widget::closeEvent(QCloseEvent *event){
    if (qProcessMap.size() > 0) {
        QString servers;
        foreach(const QString key, qProcessMap.keys()){
            servers += " <" + key + "> ";
        }
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        ui->text_log->append(tr("「%1」请停止服务 %2 后，再关闭程序。。。").arg(formattedDateTime).arg(servers));
        event->ignore(); // 阻止关闭
        return;
    } else {
        event->accept(); // 允许关闭
    }
}
