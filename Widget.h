#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFileDialog>
#include <QStringListModel>
#include <QProcess>
#include <QCloseEvent>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    // 文本列表模型
    QStringListModel *model;
    // 文本列表集合
    QStringList serverList;
    // 用来存储process对象，用于后续的销毁
    QMap<QString, QProcess*> qProcessMap;

private slots:

    void on_btn_upload_clicked();

    void on_btn_start_clicked();

    void on_btn_stop_clicked();

    void on_btn_about_clicked();

    void on_btn_restart_clicked();

    void on_btn_remove_clicked();

private:
    Ui::Widget *ui;
    // 默认服务存放位置
    QString dstFileName;
    // 服务名称
    QString serverName;
};
#endif // WIDGET_H
