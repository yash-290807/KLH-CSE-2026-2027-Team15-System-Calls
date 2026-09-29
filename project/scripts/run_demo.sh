#!/usr/bin/env bash
set -e
PORT="${1:-8080}"
echo "Start the server in one terminal:"
echo "  ./bin/server $PORT"
echo
echo "Then open 3 more terminals and run:"
echo "  ./bin/client 127.0.0.1 $PORT"
echo "in each terminal."
