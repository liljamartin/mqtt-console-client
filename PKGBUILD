# Maintainer: liljamartin <martinlilja81@protonmail.com>
pkgname=mqtt-console-client
pkgver=1.0.5
pkgrel=1
pkgdesc="A simple console-based MQTT client"
arch=('x86_64')
url="https://github.com/liljamartin/mqtt-console-client"
license=('MIT')
depends=('openssl' 'paho-mqtt-cpp' 'nlohmann-json')
makedepends=('cmake' 'make' 'gcc')
source=("$pkgname-$pkgver.tar.gz::https://github.com/liljamartin/mqtt-console-client/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('9e7ae880b569e18d8406359291d99593e0546eba6e4b512ae3e2626419f5692d') # Du kan generera korrekt sha256sum senare med `sha256sum`

build() {
    cd "$srcdir/$pkgname-$pkgver"
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
    cmake --build build
}

package() {
    cd "$srcdir/$pkgname-$pkgver/build"
    install -Dm755 mqtt_client "$pkgdir/usr/bin/mqtt_client"
    install -Dm644 "$srcdir/$pkgname-$pkgver/LICENSE" \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
    install -Dm644 "$srcdir/$pkgname-$pkgver/README.md" \
        "$pkgdir/usr/share/doc/$pkgname/README.md"
}
