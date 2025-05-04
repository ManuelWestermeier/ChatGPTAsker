const port = process.env.PORT || 3000;

require("http").createServer((req, res) => {
    res.writeHead(200, { "Content-Type": "text/plain" });
    try {
        const url = new URL(req.url, `http://localhost:${port}`);
        const query = url.searchParams.get("q");
        res.end("This is the response for " + query);
    } catch (error) {
        res.end("ERROR");
    }
}).listen(port);
