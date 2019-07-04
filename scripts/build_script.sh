mkdir /app/build
cd /app/build
cmake ../src
make
make package
mkdir -p /app/src/deb
cp /app/build/showface-*.deb /app/src/deb
