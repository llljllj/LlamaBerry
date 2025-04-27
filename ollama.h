#pragma once
#include <QTextCodec>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLabel>
#include <QPixmap>
#include <QTimer> 
// 2. 在包含 Python 头文件前取消 slots 宏
#ifdef slots
#undef slots
#endif

extern "C" {
    #include <Python.h>
}

// 3. 重新定义 slots 宏供 Qt 使用
#define slots Q_SLOTS  // 恢复宏定义


class AiChatWindow : public QWidget
{
    Q_OBJECT

public:
    AiChatWindow(QWidget *parent = nullptr);
    QNetworkReply* currentReply = nullptr;

private slots:
    void onStartClicked();
    void onSendClicked();
    void onReplyFinished(QNetworkReply *reply);
    void onProcessOutput();
    void onReplyStreamReady();
    void onVoiceInputClicked();
    void onWhisperFinished();

private:
    QPushButton *voiceButton;
    QPushButton *startButton;
    QPushButton *sendButton;
    QLineEdit *inputBox;
    QTextEdit *outputArea;
    QLabel *process_pic;
    QProcess process;
    QNetworkAccessManager *networkManager;
    QStringList history;      // 保存多轮对话
    QString lastUserInput;    // 上一次用户输入，发出去的问题
    QString fullResponse;     // 当前AI回复，拼完整流式内容
};
