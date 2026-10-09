"""Serve Encoder Drive locally using only Python 3's standard library."""
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import threading
import webbrowser


def main():
    folder = Path(__file__).resolve().parent
    handler = partial(SimpleHTTPRequestHandler, directory=str(folder))
    server = None
    for port in range(8000, 8011):
        try:
            server = ThreadingHTTPServer(("127.0.0.1", port), handler)
            break
        except OSError:
            continue
    if server is None:
        raise SystemExit("Ports 8000–8010 are unavailable. Close another local server and try again.")
    url = f"http://127.0.0.1:{server.server_port}"
    print(f"\nEncoder Drive II\nOpen in Chrome or Edge: {url}\nKeep this window open. Press Ctrl+C to stop.\n", flush=True)
    launch = threading.Timer(0.5, lambda: webbrowser.open(url))
    launch.daemon = True
    launch.start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nGame server stopped.")
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
