from flask import Flask, render_template, request, Response
import requests
import json

app = Flask(__name__)

OLLAMA_API_URL = "http://localhost:11434/api/generate"
MODEL_NAME = "qwen:0.5b"

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

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
