import logging

from config import HOST, PORT
from tcp_server import TcpServer


def main() -> None:
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    TcpServer(HOST, PORT).serve_forever()


if __name__ == "__main__":
    main()
