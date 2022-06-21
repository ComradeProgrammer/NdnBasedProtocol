
rm -f *.log
rm -f *.json
ps -aux | grep routing | awk '{ print $2 }' | xargs sudo kill
ps -aux | grep converge | awk '{ print $2 }' | xargs sudo kill
sudo mn --clean 1> /dev/null 2> /dev/null
sudo python3 main.py
ps -aux | grep routing | awk '{ print $2 }' | xargs sudo kill
sudo chmod 666 *.log
sudo chmod 666 *.json
python3 check.py