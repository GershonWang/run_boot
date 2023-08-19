#include "Widget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QScreen>
#include <QGuiApplication>

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
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QRect primaryScreenGeometry = primaryScreen->availableGeometry();
    int screenWidth = primaryScreenGeometry.width();
    int screenHeight = primaryScreenGeometry.height();
    int windowWidth = widget.width();
    int windowHeight = widget.height();
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    widget.setGeometry(0, 0, 800, 600); // 设置初始位置和大小
    // 设置窗口位置
    widget.move(x, y);
    // 渲染完毕，显示窗体
    widget.show();
    return a.exec();
}
