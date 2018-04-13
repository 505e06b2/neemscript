PORT = 8080

import SimpleHTTPServer, SocketServer, os

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    pass

Handler.extensions_map[".wasm"] = "application/wasm"

server = SocketServer.TCPServer(("", PORT), Handler)

print("Serving Neemscript at port: %u" % PORT)
os.chdir("docs")
server.serve_forever()
