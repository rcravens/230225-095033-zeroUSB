#!/bin/bash

### BEGIN INIT INFO
# Provides:          myservice
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start daemon at boot time
# Description:       Control the myservice daemon.
### END INIT INFO

prg="cam_server"
dir="/usr/sbin/"
cmd="$dir$prg"
user="bitnami"

# TLS Certs can be added with the -cert and -key options
cmd_opt="-debug -crt /home/bitnami/torks.org.crt -key /home/bitnami/torks.org.key"


pid_file="/run/$name.pid"
stdout_log="/var/log/$name.log"
stderr_log="/var/log/$name.err"

get_pid() {
    cat "$pid_file"
}

is_running() {
    [ -f "$pid_file" ] && ps `get_pid` > /dev/null 2>&1
}

case "$1" in
    start)
    if is_running; then
        echo "Already started"
    else
        echo "Starting $name"
        cd "$dir"
        if [ -z "$user" ]; then
            start-stop-daemon --start --background --pidfile "$pid_file" --make-pidfile --exec "$cmd" -- $cmd_opt >> "$stdout_log" 2>> "$stderr_log"
        else
            start-stop-daemon --start --background --pidfile "$pid_file" --make-pidfile --chuid "$user" --exec "$cmd" -- $cmd_opt >> "$stdout_log" 2>> "$stderr_log"
        fi
    fi
    ;;
    stop)
    if is_running; then
        echo "Stopping $name"
        start-stop-daemon --stop --pidfile "$pid_file" --retry 10
    else
        echo "Not running"
    fi
    rm $pid_file
    ;;
    restart)
    $0 stop
    if is_running; then
        echo "Unable to stop, will not attempt to start"
    else
        $0 start
    fi
    ;;
    status)
    if is_running; then
        echo "Running"
    else
        echo "Stopped"
    fi
    ;;
    *)
    echo "Usage: $0 {start|stop|restart|status}"
    exit 1
    ;;
esac

exit 0
