rm -f *.log
rm -f *.json
sudo mn --clean 1> /dev/null 2> /dev/null
sudo python3 main.py
sudo chmod 666 *.log
sudo chmod 666 *.json