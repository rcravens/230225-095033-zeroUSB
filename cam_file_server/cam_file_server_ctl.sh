#!/bin/bash

# Path to the Go executable
GO_APP_PATH="/path/to/cam_file_server"

start() {
    echo "Starting cam_file_server..."
    nohup $GO_APP_PATH >/dev/null 2>&1 &
    echo $! > cam_file_server.pid
    echo "cam_file_server started"
}

stop() {
    echo "Stopping cam_file_server..."
    if [ -f cam_file_server.pid ]; then
        kill $(cat cam_file_server.pid)
        rm cam_file_server.pid
        echo "cam_file_server stopped"
    else
        echo "cam_file_server is not running"
    fi
}

restart() {
    if [ -f cam_file_server.pid ]; then
        echo "Restarting cam_file_server..."
        kill $(cat cam_file_server.pid)
        rm cam_file_server.pid
        start
    else
        echo "cam_file_server is not running, starting now..."
        start
    fi
}

case $1 in
    start|stop|restart) "$1" ;;
    *) echo "Usage: $0 start|stop|restart" >&2
       exit 1
       ;;
esac
