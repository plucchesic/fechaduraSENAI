#include <Servo.h>

#include <LiquidCrystal.h>

#include <LiquidCrystal_I2C.h>

#include <Keypad.h>

#include <MFRC522.h>

#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>



LiquidCrystal_I2C lcd(0x27, 20, 4);

Servo pombo;

#define SS_PIN 53
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
//teclado embaixo
#include <Keypad.h>

String senha;
int contaSenhaErrada = 0;
char tecla;
int t;
int contaSenhaSaidaErrada = 0;
String Arroz;
String senhaNova;
String predefinicao = "1234ABCD#";
String senhaSaida;

const byte numRows = 4;
const byte numCols = 4;
int x = 0;
char keymap[numRows][numCols] = {{'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {9, 8, 7, 6};
byte colPins[numCols] = {5, 4, 3, 2};

int tt = 0;

Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

char tecla2 = myKeypad.getKey();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  pinMode(10, OUTPUT);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Boa tarde!");
  lcd.setCursor(0, 1);
  lcd.print("Posicione o RFID");

  pinMode(22, INPUT);
  pinMode(23, INPUT);

  pombo.attach(44);

}


void loop() {

  char tecla2 = myKeypad.getKey();
  int minute = 0;
  int sec = 0;
  int hora = 0;
  int alarme;

  switch (tt) {
    case 0: // put your main code here, to run repeatedly:



      pombo.write(00);
      lcd.backlight();
      lcd.setCursor(3, 0);
      lcd.print("Boa tarde!");
      lcd.setCursor(0, 1);
      lcd.print("Posicione o RFID");

      pombo.attach(44);

      delay(300);

      if (digitalRead(22)) { //abre o servo pra sair
        pombo.write(90);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Insira a senha:");
        lcd.setCursor(4, 1);
        tt = 8;
      }

      if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
      }

      if ( ! mfrc522.PICC_ReadCardSerial()) {
        tt = 1;
      }

      break;

    case 1:  //SE DETECTAR RFID
      lcd.clear();

      lcd.backlight();
      lcd.setCursor(1, 0);
      lcd.print("RFID detectado!");
      delay(1500);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Digite a senha:");
      lcd.setCursor(4, 1);
      tt = 2;
      break;


    case 2:  //PROCESSO DA SENHA




      if (tecla2 != NO_KEY) {


        lcd.print("*");
        Serial.println(tecla2);
        senha = senha + tecla2;
        senhaSaida = senhaSaida + tecla2;
        delay(200);



      }


      if (tecla2 == '#') {
        Serial.println(senhaNova);
        if (senha == predefinicao ) {
          tt = 3;
        } else if ( senha == "ACBC3412#") {
          tt = 6;
        } else if (senha == "1D*A#") { // REDEFINIÇÃO DA SENHA
          tt = 7;
          senhaNova = "";
          predefinicao = "";
          senha = "";
        } else if (senhaSaida == "48BB#") {
          tt = 9;
        }
        else {
          tt = 4;

        }
      }

      break;

    case 3:  // SE SENHA CORRETA
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Senha correta!");
      lcd.setCursor(1, 1);
      lcd.print("Seja bem-vindo!");
      pombo.write(90);
      if (digitalRead(23)) {
        pombo.write(0);
        senha = "";
        tecla2 = "";
        contaSenhaErrada = 0;
        lcd.clear();
        tt = 0;
      }
      break;

    case 4:  //SE SENHA INCORRETA
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Senha incorreta.");
      contaSenhaErrada++;
      lcd.setCursor(0, 1);
      lcd.print("Tente novamente");
      delay(1500);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Insira a senha");
      lcd.setCursor(6, 1);
      senha = "";
      if (contaSenhaErrada == 3) {
        tt = 5;
      } else {
        tt = 2;
      }

      break;

    case 5:  //SE ERROU 3 VZS
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Errou 3x a senha");
      lcd.setCursor(3, 1);
      lcd.print("30s parado.");
      delay(1500);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Tempo restante");

      lcd.setCursor(6, 1);


      for (int i = 30; i > 0; i--) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Tempo Restante");
        lcd.setCursor(6, 1);
        lcd.print(i);
        lcd.print("s");

        delay(1000);
      }

      contaSenhaErrada = 0;
      tt = 0;
      lcd.clear();

      break;
    case 6: // EMERGÊNCIA

      lcd.clear();

      lcd.setCursor(1, 0);
      lcd.print("Senha correta!");
      lcd.setCursor(1, 1);
      lcd.print("Seja bem-vindo!");
      pombo.write(90);
      for (alarme = 0; alarme < 8; alarme++) {
        digitalWrite(10, 1);
        delay(500);
        digitalWrite(10, 0);
        delay(500);
      }
      delay(1000);
      senha = "";
      tecla2 = "";
      tt = 0;
      lcd.clear();
      break;

    case 7:     //REDEFINIÇÃO
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Redefina a senha:");
      lcd.setCursor(4, 1);
      if (tecla2 != NO_KEY) {
        lcd.print("*");
        Serial.println(tecla2);
        senhaNova = senhaNova + tecla2;
        if (tecla2 == '#') {
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.print("Redefinicao");
          lcd.setCursor(4, 1);
          lcd.print("Concluida");
          delay(1000);
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Digite a senha:");
          lcd.setCursor(4, 1);
          predefinicao = senhaNova;
          Serial.println(senhaNova);
          Serial.println(predefinicao);
          tt = 2;
        }
      }
      break;

    case 8:





      if (tecla2 != NO_KEY) {


        lcd.print("*");
        Serial.println(tecla2);
        senhaSaida = senhaSaida + tecla2;
        delay(200);


      }


      if (tecla2 == '#') {

        if (senhaSaida == "48BB#") {
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.print("Ate mais!");
          contaSenhaSaidaErrada = 0;
          senhaSaida = "";
          senha = "";
          delay(2000);
          pombo.write(0);
          tt = 0;
        } else {
          tt = 10;
          senhaSaida = "";
        }
      }

      break;

    case 9: //SENHA SAÍDA CORRETA
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Ate mais!");
      senhaSaida = "";
      senha = "";
      delay(2000);
      pombo.write(0);
      senhaSaida = "";
      tt = 0;
      break;

    case 10: //SENHA SAÍDA INCORRETA
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Senha incorreta.");
      contaSenhaSaidaErrada++;
      lcd.setCursor(0, 1);
      lcd.print("Tente novamente");
      delay(1500);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Insira a senha");
      lcd.setCursor(6, 1);
      senhaSaida = "";
      if (contaSenhaSaidaErrada == 3) {
        tt = 5;
      } else {
        tt = 8;
      }

      break;

  }
}
