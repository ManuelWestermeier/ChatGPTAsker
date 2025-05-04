import http from "http";
import fetch from "node-fetch";
import dotenv from "dotenv";
dotenv.config();

const port = process.env.PORT || 8080;
const API_KEY = process.env.API_TOKEN;

http.createServer(async (req, res) => {
    console.log(`Incoming request: ${req.method} ${req.url}`);
    res.writeHead(200, { "Content-Type": "text/plain" });

    try {
        const url = new URL(req.url, `http://localhost:${port}`);
        const query = url.searchParams.get("q");

        if (!query) {
            console.log("Missing query parameter 'q'");
            res.end("Please provide a query parameter 'q'.");
            return;
        }

        console.log(`Sending query to API: "${query}"`);

        const response = await fetch("https://api-inference.huggingface.co/models/gpt2", {
            method: "POST",
            headers: {
                "Authorization": `Bearer ${API_KEY}`,
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ inputs: query })
        });

        console.log(`API responded with status: ${response.status}`);

        if (!response.ok) {
            const errorText = await response.text();
            console.error("API Error Response:", errorText);
            res.end(`API request failed with status ${response.status}`);
            return;
        }

        const data = await response.json();
        const aiResponse = data[0]?.generated_text || "No response from AI.";
        console.log("AI Response:", aiResponse);

        res.end(`AI Response: ${aiResponse}`);
    } catch (error) {
        console.error("Error during request handling:", error);
        res.end("An error occurred while processing your request.");
    }
}).listen(port, () => {
    console.log(`Server is running on port ${port}`);
});
