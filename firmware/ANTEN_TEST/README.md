# ANTEN_TEST — ESP32 yazılımı

ESP32 + Si5351 + diyot dedektörden oluşan mini anten analizörünün yazılımı.
Kurulum: [docs/KURULUM.md](../../docs/KURULUM.md) · English: [docs/BUILD.md](../../docs/BUILD.md)

- Kart: **ESP32 Dev Module**, Arduino-ESP32 çekirdeği **2.0.14** ile derlendi.
- Klasörü bütün olarak aç (`scan_config.h` ve `src/Si5351` yanında kalmalı). Ek kütüphane gerekmez.
- Si5351 kristali varsayılan **25 MHz**. Modülünde 27 MHz yazıyorsa `XTAL_HZ` satırını `27000000` yap.
- Seri port **115200 baud**, satır sonu **Yeni Satır (Newline)**.

```text
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/ANTEN_TEST
```

## Komutlar

| Komut | Ne yapar |
|---|---|
| `live 7000000 8000000 20000` | Sürekli tarama (canlı grafik sayfası bunu kendisi gönderir). Satır biçimi: `L,<başlangıç Hz>,<adım Hz>,<fark1>,<fark2>,...`. Fark = verici açıkken okuma − verici kapalıyken okuma. 5 dk sonra kendiliğinden kapanır. |
| `listen` | **Henüz denenmedi.** Verici KAPALI, yalnız dinler (ör. telefon NFC 13,56 MHz; S1 kapalıyken anten bu frekansı kapsıyor). Satır: `R,<ortalama>,<en yüksek>` her 100 ms. |
| `f7600000` | Sabit 7,6 MHz sinyal + ham okuma; 60 sn sonra kapanır. ADC voltaj kontrolü için. |
| `scan 7000000 8000000 10000` | Tek seferlik CSV tarama (her noktada kapalı/açık ölçüm). |
| `read` | Dedektörün anlık ham değeri. |
| `off` | Her şeyi durdur, çıkışları kapat. |
| `help` | Komut listesi. |

Frekans sınırı **6–8 MHz**, adım en az 100 Hz, en çok 401 nokta. (Bitiş − başlangıç) adıma tam bölünmeli.
Okunan sayılar **ham ADC farkıdır**. Güç, SWR, empedans veya verim değildir.

## Lisans

GPL-3.0-or-later ([COPYING.txt](COPYING.txt)). `src/Si5351`:
[Etherkit Si5351Arduino](https://github.com/etherkit/Si5351Arduino) (Jason Milldrum, Dana H. Myers; GPL-3.0).
`init` içindeki SYS_INIT beklemesine 1 sn zaman aşımı eklendi.
