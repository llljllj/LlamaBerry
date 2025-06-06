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

#ifdef slots
#undef slots
#endif

extern "C" {
    #include <Python.h>
}


#define slots Q_SLOTS  



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
    QLabel *process_pic;//图片
    QProcess process;
    QNetworkAccessManager *networkManager;


    //QStringList history;     //聊天记录（预留）
    //QString lastUserInput;    
    //QString fullResponse;     

};
