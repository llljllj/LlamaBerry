import vosk
from vosk import Model, KaldiRecognizer
import wave
import os
import json

# 加载模型
model_path = "../vosk-model-small-cn-0.22"
model = Model(model_path)

# 初始化识别器
rec = KaldiRecognizer(model, 16000)

# 音频路径
audio_path = 'out.wav'
output_text_path = 'out.txt'

# 检查音频是否存在
if not os.path.exists(audio_path):
    print(f"音频文件 {audio_path} 不存在！")
    exit(1)

full_text = []  # 用于累积识别内容

# 打开音频
with wave.open(audio_path, 'rb') as wave_file:
    while True:
        data = wave_file.readframes(4000)
        if len(data) == 0:
            break
        if rec.AcceptWaveform(data):
            partial_result = rec.Result()
            text = json.loads(partial_result).get('text', '')
            if text:
                full_text.append(text)

    # 最后处理 FinalResult
    final_result = rec.FinalResult()
    final_text = json.loads(final_result).get('text', '')
    if final_text:
        full_text.append(final_text)

# 保存到 out.txt
with open(output_text_path, 'w', encoding='utf-8') as f:
    f.write(' '.join(full_text))

print(f"识别内容已保存到 {output_text_path}")
