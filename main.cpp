#include <QApplication>
#include "ollama.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   qputenv("QT_IM_MODULE", QByteArray("ibus"));

   QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    AiChatWindow w;
    w.setWindowTitle("LJ的树莓派Qwen.0.5b模型");
    //w.showFullScreen();
    w.resize(1000, 800);
    w.show();

    return a.exec();
}
