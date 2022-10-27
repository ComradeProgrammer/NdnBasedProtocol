for i in $(ls /sys/class/net/) ; do
    /usr/sbin/ip addr flush $i &
done