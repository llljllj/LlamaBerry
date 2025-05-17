from flask import Flask, render_template, request, Response, send_file
from flask_cors import CORS
import requests
import json
import subprocess
import os

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
        return send_file(file_path, as_attachment=True,mimetype="video/mp4")
    else:
        return {"error": "文件不存在"}, 404

from flask import jsonify

@app.route("/files", methods=["GET"])
def list_files():
    try:
        files = os.listdir(DOWNLOAD_FOLDER)  # 获取下载目录中的文件列表
        return jsonify({"files": files})
    except Exception as e:
        return jsonify({"error": str(e)}), 500
    
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
