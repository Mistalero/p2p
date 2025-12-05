#!/usr/bin/env python3

import http.server
import socketserver
import os
import sys
import urllib.parse

# Directory where artifacts are stored
ARTIFACTS_DIR = "./artifacts"

class ArtifactHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        # Parse the URL
        parsed_path = urllib.parse.urlparse(self.path)
        path_parts = parsed_path.path.strip('/').split('/')
        
        # Check if this is a request for an artifact
        if len(path_parts) >= 2 and path_parts[0] == "artifacts":
            # Extract the artifact ID
            artifact_id = path_parts[1]
            
            # Construct the file path
            # Assuming artifacts are stored as <id>.artifact
            file_path = os.path.join(ARTIFACTS_DIR, f"{artifact_id}.artifact")
            
            # Check if the file exists
            if os.path.exists(file_path) and os.path.isfile(file_path):
                # Serve the file
                try:
                    with open(file_path, 'rb') as f:
                        content = f.read()
                    
                    # Send response
                    self.send_response(200)
                    self.send_header('Content-Type', 'application/octet-stream')
                    self.send_header('Content-Length', str(len(content)))
                    self.end_headers()
                    self.wfile.write(content)
                except Exception as e:
                    # Error reading file
                    self.send_response(500)
                    self.end_headers()
                    self.wfile.write(b"Internal server error")
            else:
                # Artifact not found
                self.send_response(404)
                self.end_headers()
                self.wfile.write(b"Artifact not found")
        else:
            # Unknown path
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b"Not found")

def main():
    # Check if artifacts directory exists
    if not os.path.exists(ARTIFACTS_DIR):
        print(f"Artifacts directory '{ARTIFACTS_DIR}' does not exist. Creating it.")
        os.makedirs(ARTIFACTS_DIR)
    
    # Start the server
    port = 8080
    handler = ArtifactHandler
    with socketserver.TCPServer(("", port), handler) as httpd:
        print(f"Artifact server running at http://localhost:{port}/")
        print(f"Serving artifacts from directory: {os.path.abspath(ARTIFACTS_DIR)}")
        print("Press Ctrl+C to stop the server")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("
Server stopped")

if __name__ == "__main__":
    main()