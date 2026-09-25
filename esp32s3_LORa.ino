#include <SPI.h>
#include <LoRa.h>

// Konfigurasi Pin LoRa RA-02 untuk ESP32-S3
const int ss = 10;
const int rst = 9;
const int dio0 = 8;

// Pin SPI fleksibel untuk ESP32-S3
const int sck = 12;
const int miso = 13;
const int mosi = 11;
const int ledPin = 20;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 0);
  while (!Serial)
    ;
  Serial.println("Inisialisasi LoRa Transceiver (Dua Arah)...");

  // Inisialisasi SPI khusus ESP32-S3
  SPI.begin(sck, miso, mosi, ss);
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {  // Frekuensi 433 MHz
    Serial.println("Gagal memulai LoRa! Periksa kabel.");
    while (1)
      ;
  }

  LoRa.setSyncWord(0xF3);  // Kode sinkronisasi (harus sama di kedua ESP32)
  Serial.println("LoRa Siap! Ketik pesan di Serial Monitor untuk dikirim.");
}

void loop() {
  // 1. CEK APAKAH ADA DATA MASUK DARI LORA
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String dataMasuk = "";
    while (LoRa.available()) {
      dataMasuk += (char)LoRa.read();
    }
    dataMasuk.trim();  // Membersihkan spasi/karakter newline

    Serial.print("Menerima pesan: ");
    Serial.print(dataMasuk);
    Serial.print(" | RSSI: ");
    Serial.println(LoRa.packetRssi());

    // Kontrol LED berdasarkan data yang diterima
    if (dataMasuk == "1") {
      digitalWrite(ledPin, HIGH);  // Nyalakan LED

      Serial.println("-> Perintah diterima: LED NYALA");
    } else if (dataMasuk == "0") {
      digitalWrite(ledPin, LOW);  // Matikan LED
      Serial.println("-> Perintah diterima: LED MATI");
    }
  }

  // 2. CEK APAKAH ADA KETIKAN DARI SERIAL MONITOR (UNTUK DIKIRIM)
  if (Serial.available() > 0) {
    String pesanKirim = Serial.readStringUntil('\n');  // Baca teks sampai enter
    pesanKirim.trim();                                 // Hapus spasi/karakter newline ekstra jika ada

    if (pesanKirim.length() > 0) {
      Serial.print("[Kirim LoRa] Mengirim: ");
      Serial.println(pesanKirim);

      // Mulai mengirim paket LoRa
      LoRa.beginPacket();
      LoRa.print(pesanKirim);
      LoRa.endPacket();

      Serial.println("Pesan terkirim ke udara!");
    }
  }
}