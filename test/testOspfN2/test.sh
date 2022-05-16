rm -f *.log
ps -aux | grep zebra | awk '{ print $2 }' | xargs sudo kill
ps -aux | grep ospfd | awk '{ print $2 }' | xargs sudo kill
sudo mn --clean 1> /dev/null 2> /dev/null
sudo python3 main.py
sudo chmod 666 *.log
python3 check.py
ps -aux | grep zebra | awk '{ print $2 }' | xargs sudo kill
ps -aux | grep ospfd | awk '{ print $2 }' | xargs sudo kill