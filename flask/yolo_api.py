from flask import send_file
import tempfile

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
    tmp = tempfile.NamedTemporaryFile(suffix='.jpg', delete=False)
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