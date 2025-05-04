const http = require("http");
const { URL } = require("url");
const fetch = require("node-fetch");

const port = process.env.PORT || 8080;

// Replace with your actual API key
const API_KEY = process.env.API_TOKEN;

http.createServer(async (req, res) => {
    res.writeHead(200, { "Content-Type": "text/plain" });

    try {
        const url = new URL(req.url, `http://localhost:${port}`);
        const query = url.searchParams.get("q");

        if (!query) {
            res.end("Please provide a query parameter 'q'.");
            return;
        }

        // Example: Using Hugging Face Inference API
        const response = await fetch("https://api-inference.huggingface.co/models/gpt2", {
            method: "POST",
            headers: {
                "Authorization": `Bearer ${API_KEY}`,
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ inputs: query })
        });

        if (!response.ok) {
            res.end(`API request failed with status ${response.status}`);
            return;
        }

        const data = await response.json();
        const aiResponse = data[0]?.generated_text || "No response from AI.";

        res.end(`AI Response: ${aiResponse}`);
    } catch (error) {
        console.error("Error:", error);
        res.end("An error occurred while processing your request.");
    }
}).listen(port, () => {
    console.log(`Server is running on port ${port}`);
});
