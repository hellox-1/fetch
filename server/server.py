from http.server import HTTPServer, BaseHTTPRequestHandler

class Server(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-Type", "text/plain")
        self.end_headers()
        self.wfile.write(b"hello sanchit")

server = HTTPServer(("10.222.242.215", 8000), Server)
print("Server running on http://10.222.242.215:8000")
server.serve_forever()
