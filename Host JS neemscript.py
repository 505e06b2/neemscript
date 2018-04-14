PORT = 8080

import SimpleHTTPServer, SocketServer, os, ssl

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    pass

Handler.extensions_map[".wasm"] = "application/wasm"

server = SocketServer.TCPServer(("", PORT), Handler)

print("Serving Neemscript at port: %u" % PORT)
server.socket = ssl.wrap_socket(server.socket, certfile="ssl.pem", server_side=True)
os.chdir("docs")
server.serve_forever()
