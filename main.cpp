#include "Widget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget widget;
    // 设置程序名称
    widget.setWindowTitle("启停boot服务");
    // 设置程序图标
    QIcon icon(":/imgs/static/Restart_128.png");
    widget.setWindowIcon(icon);
    // 获取屏幕的尺寸和中心坐标
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int windowWidth = widget.width();
    int windowHeight = widget.height();
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    // 设置窗口位置
    widget.move(x, y);
    // 渲染完毕，显示窗体
    widget.show();
    return a.exec();
}
