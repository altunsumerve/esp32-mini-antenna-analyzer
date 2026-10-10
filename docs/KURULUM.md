# Kurulum — adım adım

▶ Videolu anlatım: https://youtu.be/YRx6g13keRA

Bu rehber, bakır borudan manyetik halka anteni ve ESP32 ile yapılan mini anten analizörünü
baştan sona kurup **antenin rezonansını bulmak** için yazıldı. Her bölümün sonunda bir
**✅ Kontrol** var; o tutmadan sonraki bölüme geçme.

Bağlantı şeması: [docs/img/semalar/baglanti-semasi-tr.svg](img/semalar/baglanti-semasi-tr.svg) ·
Breadboard: [docs/img/semalar/breadboard-tr.svg](img/semalar/breadboard-tr.svg)

![Bağlantı şeması](img/semalar/baglanti-semasi-tr.png)

---

## 0. Malzeme

**Anten**

| Parça | Ölçü / not |
|---|---|
| Yumuşak bakır boru | ~1,2 m, 6 mm (klima borusu) |
| Ayar kondansatörü | Değişken kondansatör. Bu projede ev yapımı: **60–190 pF** |
| Anahtar S1 + sabit kondansatör | 3 bacaklı küçük anahtar + **330 pF** seramik (üstünde `331` yazar) |
| Zil teli | ~1,5 m (iki küçük halka + burgu bağlantılar) |
| Sap / taşıyıcı | Metal olmayan bir şey. Bu projede **Pringles kutusu** (içi köpükle dolu) |

**Elektronik**

| Kısa ad | Parça | Not |
|---|---|---|
| — | ESP32 DevKit | |
| — | Si5351 modülü | 25 MHz kristalli |
| C1, C3, C4 | 100 nF | üstünde `104` |
| C2 | 10 nF | üstünde `103` |
| R1 | 220 Ω | |
| R2 | 47–51 kΩ | bu projede 51 kΩ |
| R3 | 20 kΩ | |
| D1, D2 | BAT85 (önerilir) ya da 1N4148 | bu projede 1N4148. Diyotta **çizgili uç = katot** |
| — | Breadboard + jumper kablolar, multimetre | |

---

## 1. Büyük halka (anten)

1. Bakır boruyu **35 cm çapında** bir daireye yavaşça bük. Boruyu ezme.
2. Tepede iki uç arasında **~1 cm boşluk** bırak. Sol uca **A**, sağ uca **B** de.
3. Halkayı metal olmayan bir sapa sabitle. Bu projede halkanın alt kısmı bir **Pringles kutusunun**
   iki yanından delinip içinden geçirildi; kutu "meşale sapı" gibi tutuluyor ve masada dik duruyor.
4. A ile B birbirine değmesin; boşluk oynamasın.

### Ayar kondansatörü
1. **Bağlamadan önce:** multimetre ötme kademesinde kondansatörün iki ucunu ölç, ayarını baştan
   sona gezdir. **Hiç ötmemeli** (ötüyorsa plakalar değiyor).
2. Bir ucunu **A**'ya, diğerini **B**'ye bağla. Kablolar olabildiğince kısa olsun.

<img src="img/fotograflar/ayar-kondansatoru-plaketler.jpg" width="280"> <img src="img/fotograflar/anten-genel-etiketli.jpg" width="280">

*Bu projedeki ev yapımı kondansatör bakır plaketlerden; uçları krokodil kıskaçlarla A ve B'ye bağlı.*

### S1 + 330 pF
- 3 bacaklı anahtarın **orta bacağı → A**, bir **kenar bacağı → 330 pF → B**.
- S1 açıkken 330 pF ayar kondansatörüne paralel eklenir, anten daha düşük frekansa iner.

<img src="img/fotograflar/s1-anahtar-330pF.jpg" width="260">

| S1 | Antenin ayarlanabildiği aralık (hesap, L ≈ 0,9 µH) |
|---|---|
| Kapalı (330 YOK) | ~11,8 – 19,9 MHz — tarama penceresinin (6–8 MHz) **dışında** |
| Açık (330 VAR) | ~7,2 – 8,3 MHz — tarama penceresinin **içinde** |

Bu fark rezonansı bulmanın anahtarı (bkz. 7. bölüm).

### ✅ Kontrol 1
- A – B arası (bakır üzerinden) **öter**.
- Ayar kondansatörü bağlanmadan önce kendi uçları arasında **ötmedi**.

---

## 2. İki küçük halka: TX (verici) ve RX (alıcı)

Her biri **zil telinden 7 cm çaplı, tek tur** (bir kola kutusuna bir tur sarıp çıkar).
Uçları birbirine **değmez**; iki ucu **sıkıca birbirine burularak** (burgu) devreye gider.

| Halka | Nerede | Uçları nereye |
|---|---|---|
| **TX halka** | Büyük halkanın **içinde, altta**, bakıra **değmeden** (5–10 mm boşluk), aynı düzlemde | ucu 1 → **V** düğümü (R1'in çıkışı), ucu 2 → **GND** |
| **RX halka** | Büyük halkanın **dışında**, TX halkanın **tam karşısında, ~15 cm önde**, aynı yükseklikte | ucu 1 → **T** düğümü (D1'in çizgisiz ucu), ucu 2 → **GND** |

Yandan görünüş için breadboard çizimindeki "ANTEN — yandan" kutusuna bak.

![TX ve RX halkalarının gerçek yerleşimi](img/fotograflar/genel-duzenek-etiketli.jpg)

*Bu projede TX halka büyük halkanın içinde, ahşap desteğin yanında; RX halka dışarıda, ikinci bir kutunun üstünde köpüğe tutturulmuş.*

- Küçük halkalardan büyük halkanın A/B uçlarına **hiç tel gitmez**. Sinyal havadan geçer
  (karşılıklı indüksiyon).
- Burgu yapılmazsa açık teller de anten gibi sinyal toplar ve ölçümü bozar.

### ✅ Kontrol 2
- Her küçük halkanın iki ucu arası **öter** (halka tek parça tel, normal).
- Küçük halkalar ile büyük halka arası **ötmez**.

---

## 3. Breadboard bağlantıları

Breadboard çizimindeki "Takma sırası"nı izle. Aşağıda her bağlantı tek tek:

### 3a. Güç ve raylar
| Nereden | Nereye |
|---|---|
| ESP32 **3V3** | breadboard **+** rayı |
| ESP32 **GND** | breadboard **−** rayı |
| Üst ve alt raylar | sağ uçtaki iki köprü kabloyla birbirine (+ ile +, − ile −) |

### 3b. Si5351
| Nereden | Nereye |
|---|---|
| Si5351 **VIN** | **+** ray (3,3 V) — **5 V verme!** |
| Si5351 **GND** | **−** ray |
| Si5351 **SDA** | ESP32 **GPIO21** |
| Si5351 **SCL** | ESP32 **GPIO22** |
| **C4** 100 nF | Si5351 VIN ile GND arasına (modüle yakın) |

CLK1 ve CLK2 boş kalır.

### 3c. Verici (TX)
| Nereden | Nereye |
|---|---|
| Si5351 **CLK0** | **C1** (100 nF) bir ucu |
| C1 diğer ucu (düğüm **X**) | **R1** (220 Ω) bir ucu |
| R1 diğer ucu (düğüm **V**) | **TX halka** ucu 1 (burgu telin bir damarı) |
| TX halka ucu 2 | **GND** |

### 3d. Dedektör (alıcı)
| Nereden | Nereye |
|---|---|
| **RX halka** ucu 1 | düğüm **T** |
| RX halka ucu 2 | **GND** |
| T | **D1** çizgisiz ucu (anot) |
| D1 **çizgili** ucu (katot) | düğüm **DET** |
| DET | **R2** (47–51 kΩ) → GND |
| DET | **C2** (10 nF) → GND |
| DET | **R3** (20 kΩ) → düğüm **ADC** |
| ADC | **C3** (100 nF) → GND |
| ADC | **D2** çizgisiz ucu; D2 **çizgili** ucu → **3V3** |
| ADC | ESP32 **GPIO34** — ⚠️ **EN SON**, 5. bölümdeki voltaj kontrolünden sonra |

**Bütün GND'ler ortak** (ESP32 GND, Si5351 GND, halka uçları, R2/C2/C3).

<img src="img/fotograflar/breadboard-gercek-etiketli.jpg" width="360">

*Gerçek breadboard: T, DET ve ADC düğümleri renkli kısa kablolarla işaretli (yeşil, sarı, mavi).*

### ✅ Kontrol 3 (enerjisiz, multimetre)
- 3V3 – GND arası **ötmez** (kısa devre yok).
- D1 ve D2 diyot kademesinde bir yönde ~0,3 V (BAT85) / ~0,6 V (1N4148), ters yönde **OL**.
- T – GND **öter** (RX halka üzerinden), V – GND **öter** (TX halka üzerinden).

---

## 4. Yazılımı yükle (GPIO34 hâlâ bağlı değil)

1. Arduino IDE → Kart Yöneticisi → **esp32** (2.0.14 ile denendi).
2. `firmware/ANTEN_TEST/ANTEN_TEST.ino` dosyasını aç (klasör bütün kalsın).
3. Kart: **ESP32 Dev Module** → Yükle.
4. Seri ekran **115200 baud**, **Yeni Satır** → `# READY; RF OFF.` görmelisin.
   Görmüyorsan Si5351 bağlantısını (VIN, GND, SDA, SCL) kontrol et.

---

## 5. Voltaj kontrolü — ESP32'yi korumak için

Multimetre **DC volt**; siyah prob GND, kırmızı prob **ADC** düğümünde:

1. Seri ekrana `off` → değer 0'a yakın.
2. S1 açık, `f7600000` yaz, ayar kondansatörünü baştan sona gezdir.
3. Değer **pozitif ve 2,8 V'un altında** kalmalı. Aşarsa USB'yi çıkar, bağlantıları kontrol et.
4. `off` → USB'yi çıkar → **ADC'yi GPIO34'e bağla** → USB'yi tak.

---

## 6. Canlı grafik

1. Arduino seri ekranını **kapat** (port aynı anda tek programa açılır).
2. `web/canli-grafik.html` dosyasını **Chrome** ya da **Edge** ile aç.
3. **"1) ESP32'ye bağlan"** → ESP32'nin portunu seç → `READY` yazısını bekle.
4. "Tarama aralığı": Başlangıç **7,0**, Bitiş **8,0**, Adım **20** kHz.
5. **⛰️ Tarama**. Yeşil çizgi her taramada yenilenir, kırmızı soluk çizgi tepeleri hatırlar.
   "Son 8 taramanın ortalaması" açık kalsın (gürültüyü azaltır).

---

## 7. Rezonansı bulmak (bu projede işe yarayan yöntem)

TX halkadan RX halkaya sinyalin bir kısmı **doğrudan** da gelir; büyük halka olmasa bile
grafik boş kalmaz. Bu "doğrudan sızıntıyı" çıkarmak için **referans** kullanılır:

1. **S1 kapalı (330 YOK)** → antenin rezonansı tarama penceresinin dışında (~12 MHz üstü).
   Tarama çalışsın, birkaç tarama bekle (ortalama otursun).
2. **"Referans kaydet"** → sayfa bu eğriyi "antensiz durum" olarak saklar.
   "Referansı çıkar" işaretli olsun → grafik **düz, sıfır civarı** olmalı.
3. **S1'i aç (330 VAR)** → artık anten pencerenin içinde. Grafik referanstan farkı gösterir:
   büyük halkanın katkısı **bir tepe ("dağ")** olarak çıkar. Büyük yazı tepenin frekansını gösterir.
4. **Ayar kondansatörünü yavaşça çevir** → tepe kaymalı: kapasite artınca sola (düşük frekans),
   azalınca sağa.
5. İnce tarama için adımı (ör. 5 kHz) değiştirirsen **referansı aynı adımda yeniden kaydet**:
   sayfa, aralık ya da adım değişince eski referansı siler.

**Bu projede ölçülen:** S1 açıkken ~**7,62 MHz** civarında geniş bir tepe; ayar düğmesi çevrilince tepe
~7,29 → 7,14 → 7,07 MHz'e kaydı (bkz. [ölçüm ekran görüntüleri](../README.tr.md#ölçüm-sonuçları)).
Hesaplanan pencere (7,2–8,3 MHz) ile uyumlu. Tepe pencerenin kenarına geldiğinde başlangıcı 6,5 MHz yapıp
tepenin tamamını görebilirsin.

### Başarı ne demek?
- S1 kapalı + referans çıkarılmış → **düz**.
- S1 açık → **tepe** çıkıyor.
- Ayar düğmesiyle tepe **kayıyor**.

Bu üçü, tepenin büyük halkadan (antenin rezonansından) geldiğini gösterir.

### Sınırlar
- Bu bir **göreli** test. Tepenin frekansı rezonansın **yaklaşık** yeridir; SWR, empedans, verim ölçmez.
- Yazılım 6–8 MHz ile sınırlı.
- Tepe genişse: ev yapımı kondansatörün kayıpları, 1N4148'in eşiği ve halkaların konumu etkiler.

---

## Sorun giderme

| Belirti | Bak |
|---|---|
| `READY` gelmiyor | Si5351 VIN/GND/SDA/SCL, 3,3 V |
| Grafik hep sıfır | D1 yönü, RX halka bağlantısı, GPIO34 kablosu |
| Grafik düz ve yüksek, S1 ile değişmiyor | Tek halkalı (yansıma) bağlantı kalmış olabilir; TX ve RX ayrı mı? |
| Referans sonrası S1 açınca da düz | Ayar kondansatörünü gezdir; aralığı 6,0–8,0 MHz yap |
| Tepe hep kenarda | Düğmeyi çevir ya da aralığı kaydır |
| "Bağlanamadı" | Arduino seri ekranı açık kalmış olabilir; kapat |
