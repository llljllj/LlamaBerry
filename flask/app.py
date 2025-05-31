# ...existing code...
from flask import Flask, render_template, request, Response, send_file, jsonify
from flask_cors import CORS
import requests
import json
import subprocess
import os
import tempfile
import io
from PIL import Image

# YOLOv5模型加载（确保已安装 yolov5 依赖和 torch）
import torch
model = torch.hub.load('ultralytics/yolov5', 'yolov5s', device='cpu')  # 或 'cuda'，如有GPU

app = Flask(__name__)
CORS(app)

# 增加超时时间
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0
OLLAMA_API_URL = "http://localhost:11434/api/generate"
MODEL_NAME = "qwen:0.5b"
UPLOAD_FOLDER = "../src/picture" 
DOWNLOAD_FOLDER = "../src/picture"
os.makedirs(DOWNLOAD_FOLDER, exist_ok=True)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/chat", methods=["POST"])
def chat():
    data = request.get_json()
    user_message = data.get("message", "")

    def generate():
        try:
            with requests.post(
                OLLAMA_API_URL,
                json={
                    "model": MODEL_NAME,
                    "prompt": user_message,
                    "stream": True
                },
                headers={"Content-Type": "application/json"},
                stream=True,
                timeout=60
            ) as response:
                for line in response.iter_lines():
                    if line:
                        try:
                            content = json.loads(line.decode("utf-8"))
                            chunk = content.get("response", "")
                            yield f"data: {chunk}\n\n"
                        except Exception:
                            pass
        except Exception as e:
            yield f"data: [连接错误：{str(e)}]\n\n"

    return Response(generate(), mimetype="text/event-stream")

@app.route("/upload", methods=["POST"])
def upload_file():
    if "file" not in request.files:
        return {"error": "No file part"}, 400

    file = request.files["file"]
    if file.filename == "":
        return {"error": "No selected file"}, 400

    # 保存文件到临时目录
    file_path = os.path.join(UPLOAD_FOLDER, file.filename)
    file.save(file_path)

    # 将文件发送到树莓派上的 HTTP 服务器
    try:
        with open(file_path, "rb") as f:
            response = requests.post(
                "http://localhost:8080/upload",  # 树莓派 HTTP 服务器地址
                files={"file": f}
            )
        return response.json()  # 返回树莓派服务器的响应
    except Exception as e:
        return {"error": str(e)}, 500
    finally:
        os.remove(file_path)  # 删除临时文件

@app.route("/download/<filename>", methods=["GET"])
def download_file(filename):
    file_path = os.path.join(DOWNLOAD_FOLDER, filename)
    if os.path.exists(file_path):
        return send_file(file_path, as_attachment=True, mimetype="video/mp4")
    else:
        return {"error": "文件不存在"}, 404

@app.route("/files", methods=["GET"])
def list_files():
    try:
        files = os.listdir(DOWNLOAD_FOLDER)  # 获取下载目录中的文件列表
        return jsonify({"files": files})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route("/system_status", methods=["GET"])
def system_status():
    try:
        resp = requests.get("http://localhost:8090/status", timeout=2)
        return resp.json()
    except Exception as e:
        return {"error": f"获取系统状态失败: {str(e)}"}, 500

@app.route('/yolo_detect', methods=['POST'])
def yolo_detect():
    if 'file' not in request.files:
        return jsonify({'error': 'No file uploaded'}), 400
    file = request.files['file']
    img_bytes = file.read()
    img = Image.open(io.BytesIO(img_bytes))

    # 推理
    results = model(img)
    detections = results.pandas().xyxy[0].to_dict(orient="records")

    # 保存带框图片到临时文件
    results.render()  # 在原图上画框
    img_with_boxes = Image.fromarray(results.ims[0])
    tmp = tempfile.NamedTemporaryFile(suffix='.jpg', dir='/tmp', delete=False)
    img_with_boxes.save(tmp.name)
    tmp.close()

    # 返回检测数据和图片URL
    return jsonify({
        'detections': detections,
        'img_url': f'/detect_image/{os.path.basename(tmp.name)}'
    })

@app.route('/detect_image/<filename>')
def detect_image(filename):
    return send_file(f'/tmp/{filename}', mimetype='image/jpeg')

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=1230, debug=True)