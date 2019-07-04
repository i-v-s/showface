docker build -t qt-builder .
docker run -it --rm -v $(pwd)/..:/app/src qt-builder sh /app/src/scripts/build_script.sh
# -it: Keep STDIN open even if not attached & Allocate a pseudo-tty
# --rm: Automatically remove the container when it exits

