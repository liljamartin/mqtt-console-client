# Maintainer: Martin Lilja <you@example.com>
pkgname=mqtt-console-client
pkgver=1.0.5
pkgrel=1
pkgdesc="A simple console-based MQTT client"
arch=('x86_64')
url="https://github.com/liljamartin/mqtt_console_client"
license=('MIT')
depends=('openssl' 'paho-mqtt-cpp' 'nlohmann-json')
makedepends=('cmake' 'make' 'gcc')
source=("$pkgname-$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$srcdir/$pkgname-$pkgver"
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
    cmake --build build
}

package() {
    cd "$srcdir/$pkgname-$pkgver/build"
    install -Dm755 mqtt_client "$pkgdir/usr/bin/mqtt_client"
    install -Dm644 ../README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
    install -Dm644 ../LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
