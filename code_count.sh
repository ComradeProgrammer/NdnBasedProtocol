code=$(find src -name "*.cpp" -or -name "*.h" | xargs wc -l| tail -n 1)
test=$(find src -name "*.cc" | xargs wc -l| tail -n 1)
echo "source code lines:${code}"
echo "test   code lines:${test}"