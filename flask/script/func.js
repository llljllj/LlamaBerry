const chatBox = document.getElementById("chat-box");

function appendMessage(role, text, id = null) {
    const msg = document.createElement("div");
    msg.className = `message ${role}`;
    msg.innerHTML = `<strong>${role === "user" ? "你" : "AI"}:</strong> <span id="${id || ''}">${text}</span>`;
    chatBox.appendChild(msg);
    chatBox.scrollTop = chatBox.scrollHeight;
}

async function sendMessage() {
    const input = document.getElementById("message");
    const userText = input.value.trim();
    if (!userText) return;

    appendMessage("user", userText);
    input.value = "";

    const aiId = "ai-" + Date.now();
    appendMessage("ai", "...", aiId);

    try {
        const response = await fetch("/chat", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ message: userText })
        });

        const reader = response.body.getReader();
        const decoder = new TextDecoder("utf-8");
        let fullText = "";

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;
            const chunk = decoder.decode(value);
            fullText += chunk.replace(/^data:\s*/gm, "");
            document.getElementById(aiId).textContent = fullText;
        }
    } catch (err) {
        document.getElementById(aiId).textContent = "（错误：无法连接AI服务）";
    }
}