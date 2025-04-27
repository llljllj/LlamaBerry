#include <QApplication>
#include "ollama.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   // 设置输入法模块
   qputenv("QT_IM_MODULE", QByteArray("ibus"));

   // 设置全局编码为UTF-8
   QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    AiChatWindow w;
    w.setWindowTitle("LJ的树莓派Qwen.0.5b模型");
    //w.showFullScreen();
    w.resize(1000, 800);
    w.show();

    return a.exec();
}
