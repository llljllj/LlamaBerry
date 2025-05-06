#include "ollama.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QDebug>
#include <QThread>
#include <QDir>

AiChatWindow::AiChatWindow(QWidget *parent) : QWidget(parent)//初始化
{

    voiceButton = new QPushButton("语音转文字", this);
    connect(voiceButton, &QPushButton::clicked, this, &AiChatWindow::onVoiceInputClicked);


    startButton = new QPushButton("启动Qwen", this);
    inputBox = new QLineEdit(this);
    inputBox->setPlaceholderText("输入你的问题");

    sendButton = new QPushButton("发送", this);
    outputArea = new QTextEdit(this);
    outputArea->setReadOnly(true);
    process_pic = new QLabel(this);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(startButton);
    vlayout->addWidget(inputBox);
    vlayout->addWidget(sendButton);
    vlayout->addWidget(outputArea);
<<<<<<< HEAD
    vlayout->addWidget(voiceButton); 
=======
    vlayout->addWidget(voiceButton);  
>>>>>>> 86268d670f369f8cbd5e1a333e1c484dddc77b0b
    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->addLayout(vlayout);
    hlayout->addWidget(process_pic);

    process_pic->setStyleSheet("border: 2px solid red;");
    QPixmap pixmap("/home/lllj/Desktop/ollama_qt_cpp/LlamaBerry/1.jpg");
    process_pic->setPixmap(pixmap);
    process_pic->setFixedSize(600, 800); 
<<<<<<< HEAD
    process_pic->setScaledContents(true); // 图片填充

=======
    process_pic->setScaledContents(true); 
>>>>>>> 86268d670f369f8cbd5e1a333e1c484dddc77b0b
    this->setStyleSheet(R"(
        QWidget {
            background-color: #0f111a;  
            color: #ffffff;             
            font-family: 'Roboto', 'Noto Sans CJK SC', sans-serif;
            font-size: 14px;
        }
    
        QPushButton {
            min-height: 36px;
            padding: 8px 16px;
            border-radius: 8px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #00c6ff, stop:1 #0072ff);
            color: white;
            font-weight: bold;
            border: 1px solid #00c6ff;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #0072ff, stop:1 #00c6ff);
        }
        QPushButton:pressed {
            background-color: #003d99;
        }
    
        QLineEdit {
            background: rgba(255,255,255,0.05);
            border: 1px solid #00c6ff;
            border-radius: 8px;
            padding: 8px;
            color: #00eaff;
            selection-background-color: #005f87;
            selection-color: white;
        }
    
        QTextEdit {
            background: rgba(255,255,255,0.05);
            border: 1px solid #00c6ff;
            border-radius: 8px;
            padding: 10px;
            color: #00eaff;
            font-family: Consolas, 'Courier New', monospace;
            font-size: 14px;
        }
    
        QScrollBar:vertical {
            width: 8px;
            background: #1e1f29;
        }
        QScrollBar::handle:vertical {
            background: #00c6ff;
            min-height: 20px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical:hover {
            background: #00eaff;
        }
        
    )");
    
    
    networkManager = new QNetworkAccessManager(this);
    inputBox->setAttribute(Qt::WA_InputMethodEnabled, true);
    connect(inputBox, &QLineEdit::returnPressed, this, &AiChatWindow::onSendClicked);
    connect(startButton, &QPushButton::clicked, this, &AiChatWindow::onStartClicked);
    connect(sendButton, &QPushButton::clicked, this, &AiChatWindow::onSendClicked);
    connect(networkManager, &QNetworkAccessManager::finished, this, &AiChatWindow::onReplyFinished);
}


void AiChatWindow::onVoiceInputClicked()//启动语音识别
{
    outputArea->append("开始录音中...");

    //录音
    QString recordCommand = "arecord -D hw:0,0 -f S16_LE -r 16000 -c 1 -d 5 /home/lllj/Desktop/ollama_qt_cpp/LlamaBerry/src/voice/out.wav";
    QProcess::execute(recordCommand);

    outputArea->append("录音结束，正在识别...");

    //脚本进行语音识别
    QString pythonScript = "/home/lllj/Desktop/ollama_qt_cpp/LlamaBerry/vosk/vosktest.py";
    QProcess process;
    process.start("python3", QStringList() << pythonScript);
    process.waitForFinished(); 
    QString outputPath = "/home/lllj/Desktop/ollama_qt_cpp/LlamaBerry/src/txt/out.txt";
    QFile file(outputPath);

    int timeout = 5000; //等待时间
    int elapsed = 0;
    while (!file.exists() && elapsed < timeout) {
        QThread::msleep(100);
        elapsed += 100;
    }

    if (!file.exists()) {
        outputArea->append("未生成 out.txt 文件");
        return;
    }

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString recognizedText = file.readAll();
        outputArea->append("语音识别结果：" + recognizedText);

        //发送问题
        inputBox->setText(recognizedText);
        onSendClicked(); 

        file.close();

        // 识别完删除 out.txt
        file.remove();
    } else {
        outputArea->append("无法读取识别结果");
    }
}


void AiChatWindow::onWhisperFinished()//语音识别结束
{
    QString textPath = QDir::homePath() + "/Desktop/ollama_qt_cpp/LlamaBerry/src/txt/input.wav.txt";
    QFile file(textPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll()).trimmed();
        file.close();

        if (!content.isEmpty()) {
            inputBox->setText(content);
            outputArea->append("识别结果: " + content);
<<<<<<< HEAD
            onSendClicked();
=======
            onSendClicked(); 
>>>>>>> 86268d670f369f8cbd5e1a333e1c484dddc77b0b
        } else {
            outputArea->append("语音识别失败，空内容");
        }
    } else {
        outputArea->append("无法读取识别结果文件");
    }
}


void AiChatWindow::onStartClicked()//启动大模型
{
    QString scriptPath = QDir::homePath() + "/Desktop/ollama-chatgpt-web/run_ollama.sh";
    process.setWorkingDirectory(QDir::homePath() + "/Desktop/ollama-chatgpt-web");
    process.start(scriptPath);


    if (process.waitForStarted()) {
        outputArea->append(" 已尝试启动大模型");
        connect(&process, &QProcess::readyReadStandardOutput, this, &AiChatWindow::onProcessOutput);
    } else {
        outputArea->append(" 启动失败！请检查路径或权限。");
    }
}


void AiChatWindow::onProcessOutput()//输出ai启动的日志（bug）
{
    QByteArray output = process.readAllStandardOutput();//读取ai在终端的输出
    outputArea->append("[AI 启动日志] " + QString::fromUtf8(output));
}

void AiChatWindow::onSendClicked()//发送消息
{
    QString userInput = inputBox->text().trimmed();
    if (userInput.isEmpty()) {
        outputArea->append("请输入内容再发送");
        return;
    }

    outputArea->append("你： " + userInput+"\n"+"LJ:");
    inputBox->clear();
    //图片
    QPixmap pixmap("/home/lllj/Desktop/ollama_qt_cpp/LlamaBerry/src/picture/think.jpg");
    process_pic->setPixmap(pixmap);
    process_pic->setFixedSize(600, 800); 
<<<<<<< HEAD
    process_pic->setScaledContents(true);

    QJsonObject json;
    json["model"] = "qwen:0.5b";
    json["prompt"] = userInput;
    json["stream"] = true;  //开启流式

    QNetworkRequest request(QUrl("http://localhost:11434/api/generate"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    //发请求
    currentReply = networkManager->post(request, data);

    //处理流式响应
    connect(currentReply, &QNetworkReply::readyRead, this, &AiChatWindow::onReplyStreamReady);

    //释放资源
    connect(currentReply, &QNetworkReply::finished, currentReply, &QNetworkReply::deleteLater);
}


void AiChatWindow::onReplyStreamReady()//流式处理
{
    static QString pendingText;//缓冲区，处理未显示的字符
    static QTimer *timer = nullptr;

    while (currentReply->bytesAvailable()) {
        QByteArray chunk = currentReply->readLine().trimmed();
        if (chunk.isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(chunk, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            QString content = doc["response"].toString();
            pendingText += content;
        }
    }

    if (!timer) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [=]() mutable {
            if (!pendingText.isEmpty()) {
                QString nextChar = pendingText.left(1);
                pendingText.remove(0, 1);
                outputArea->moveCursor(QTextCursor::End);
                outputArea->insertPlainText(nextChar);
            } else {
                timer->stop();
            }
        });
    }

    if (!timer->isActive()) {
        timer->start(20); //每20毫秒打一字

    }
}

void AiChatWindow::onReplyFinished(QNetworkReply *reply)//结果处理（非流式消息返回，已废除）
{
    if (reply->error()) {
        outputArea->append("\n网络错误：" + reply->errorString());
    } else {
        outputArea->append("\n[流式回复结束]\n");
        QPixmap pixmap("/home/lllj/Desktop/ollama_qt_cpp/LlamaBerry/src/picture/1.jpg");
        process_pic->setPixmap(pixmap);
        process_pic->setFixedSize(600, 800); 
        process_pic->setScaledContents(true); 
    }

    reply->deleteLater();
}
