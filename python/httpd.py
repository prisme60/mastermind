from http.server import *
from http.server import CGIHTTPRequestHandler

##class MyOwnHTTPHandler(SimpleHTTPRequestHandler):
##    def do_HEAD():
##        pass
##
##    def do_GET():
##        pass
##
##    def do_POST():
##        pass

class SimpleCGIHTTPRequestHandler(CGIHTTPRequestHandler):
    def guess_type(self, path):
##        if path[-4:] == ".svg":
##            print("image/xml+svg")
##            return "image/xml+svg"
##        elif path[-5:] == ".svgz":
##            print("image/xml+svg")
##            return "image/xml+svg"
##        else:
            mimetype = CGIHTTPRequestHandler.guess_type(self, path)
            print(mimetype)
            return mimetype


def run(server_class=HTTPServer, handler_class=SimpleCGIHTTPRequestHandler):
    server_address = ('', 8080)
    httpd = server_class(server_address, handler_class)

    sa = httpd.socket.getsockname()
    print("Serving HTTP on", sa[0], "port", sa[1], "...")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nKeyboard interrupt received, exiting.")
        httpd.server_close()
        sys.exit(0)

if __name__ == "__main__":
    run()
    
