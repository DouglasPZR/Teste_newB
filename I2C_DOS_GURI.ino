//--------------------------------------------------------------------- INCLUDES
#include <Wire.h>

//tetse git

//--------------------------------------------------------------------- DEFINES
#define EXPANDER_SAIDA_GIGA 0x23      //saida placa teste
#define EXPANDER_ENTRADA_GIGA 0x27    //entrada placa teste
#define EXPANDER_SAIDA_PLACA_B 0x21   //saida placa New B
#define EXPANDER_ENTRADA_PLACA_B 0x20 //Entrada placa New B

#define EXPANDER_GP0 0x00     // DATA PORT REGISTER 0
#define EXPANDER_GP1 0x01     // DATA PORT REGISTER 1
#define EXPANDER_OLAT0 0x02   // OUTPUT LATCH REGISTER 0
#define EXPANDER_OLAT1 0x03   // OUTPUT LATCH REGISTER 1
#define EXPANDER_IPOL0 0x04   // INPUT POLARITY PORT REGISTER 0
#define EXPANDER_IPOL1 0x05   // INPUT POLARITY PORT REGISTER 1
#define EXPANDER_IODIR0 0x06  // I/O DIRECTION REGISTER 0
#define EXPANDER_IODIR1 0x07  // I/O DIRECTION REGISTER 1
#define EXPANDER_INTCAP0 0x08 // INTERRUPT CAPTURE REGISTER 0
#define EXPANDER_INTCAP1 0x09 // INTERRUPT CAPTURE REGISTER 1
#define EXPANDER_IOCON0 0x0A  // I/O EXPANDER CONTROL REGISTER 0
#define EXPANDER_IOCON1 0x0B  // I/O EXPANDER CONTROL REGISTER 1

//---------------------------------- SETUP-----------------------------------
void setup()
{
  Serial.begin(9600);
  delay(1000);
  Wire.begin(18, 19);    //i2c SDA - 18, SCL - 19
  Wire.setClock(100000); //frêquencia

  //---------------------------------------- CONFIGURA EXPANDER 1 Saida placa teste
  i2c_configura_portas(EXPANDER_SAIDA_GIGA, EXPANDER_IODIR0, OUTPUT);
  i2c_configura_portas(EXPANDER_SAIDA_GIGA, EXPANDER_IODIR1, OUTPUT);

  //---------------------------------------- CONFIGURA EXPANDER 2 Entrada placa teste
  i2c_configura_portas(EXPANDER_ENTRADA_GIGA, EXPANDER_IODIR0, OUTPUT);
  i2c_configura_portas(EXPANDER_ENTRADA_GIGA, EXPANDER_IODIR1, INPUT);

  //---------------------------------------- CONFIGURA EXPANDER 3 Saida placa new B
  i2c_configura_portas(EXPANDER_SAIDA_PLACA_B, EXPANDER_IODIR0, OUTPUT);
  i2c_configura_portas(EXPANDER_SAIDA_PLACA_B, EXPANDER_IODIR1, OUTPUT);

  //---------------------------------------- CONFIGURA EXPANDER 4 Entrada placa new B
  i2c_configura_portas(EXPANDER_ENTRADA_PLACA_B, EXPANDER_IODIR0, INPUT);
  i2c_configura_portas(EXPANDER_ENTRADA_PLACA_B, EXPANDER_IODIR1, INPUT);

  //-----------------------------------------
  i2c_escreve_pinos(EXPANDER_SAIDA_GIGA, EXPANDER_GP0, 0);
  i2c_escreve_pinos(EXPANDER_SAIDA_GIGA, EXPANDER_GP1, 0);
}

//---------------------------------- LOOP -----------------------------------
void loop()
{

  testa_saidas_placa_B();

  /*   uint8_t leitura_pino = 0;

  leitura_pino = i2c_ler_pino_unico(EXPANDER_ENTRADA_GIGA, EXPANDER_GP1, 7);
  if (leitura_pino == 1)
    i2c_escreve_pino_unico(EXPANDER_ENTRADA_GIGA, EXPANDER_GP0, 5, 1);
  else if (leitura_pino == 0)
    i2c_escreve_pino_unico(EXPANDER_ENTRADA_GIGA, EXPANDER_GP0, 5, 0);

  i2c_escreve_pinos(EXPANDER_SAIDA_GIGA, EXPANDER_GP0, 0);
  i2c_escreve_pinos(EXPANDER_SAIDA_GIGA, EXPANDER_GP1, 0);
  delay(500); */
}

//---------------------------------- CONFIGURA EXPANDER  -----------------------------------
void i2c_configura_portas(uint8_t endereco, uint8_t comando, uint8_t valor)
{
  if (valor == INPUT)
  {
    i2c_escreve_pinos(endereco, comando, 0xFF);
  }
  else if (valor == OUTPUT)
  {
    i2c_escreve_pinos(endereco, comando, 0x00);
  }
  else
  {
    i2c_escreve_pinos(endereco, comando, valor);
  }
}

//---------------------------------- ESCREVE PINO UNICO -----------------------------------
// i2c_escreve_pino_unico (endereço_da_expander, comando_expander, pino_escrita, valor)
void i2c_escreve_pino_unico(uint8_t endereco, uint8_t comando, int pino, int valor)
{
  uint8_t statusGP = 0;
  Wire.beginTransmission(endereco);
  Wire.write(comando);
  Wire.endTransmission();
  Wire.requestFrom(endereco, 1); //requisita a leitura de 1 byte vindo do GP para saber o status dos pinos da porta
  statusGP = Wire.read();

  if (valor == 0)
  {                             //se o valor a setar for ZERO fazemos uma operação de subtração binária
    statusGP &= ~(1 << (pino)); // muda o pino passado para ZERO (LOW)
  }

  else if (valor == 1)
  {                            //caso seja UM, faremos uma operação de soma binária
    statusGP |= (1 << (pino)); // muda o pino passado para UM (HIGH)
  }

  Wire.beginTransmission(endereco);
  Wire.write(comando);
  Wire.write(statusGP);
  Wire.endTransmission();
}

//---------------------------------- ESCREVE VARIOS PINOS -----------------------------------
// i2c_escreve_pinos (endereço_da_expander, comando_expander, valor_portas)
void i2c_escreve_pinos(uint8_t endereco, uint8_t comando, uint8_t dados)
{
  Wire.beginTransmission(endereco);
  Wire.write(comando);
  Wire.write(dados);
  Wire.endTransmission();
  delay(10);
}

//---------------------------------- LEITURA PINO UNICO -----------------------------------
// i2c_ler_pino_unico (endereço_da_expander, comando_expander, valor_portas)
uint8_t i2c_ler_pino_unico(uint8_t endereco, uint8_t porta, uint8_t pino)
{
  uint8_t statusGP = 0;

  Wire.beginTransmission(endereco);
  Wire.write(porta);
  Wire.endTransmission();
  Wire.requestFrom(endereco, 1); // ler do chip  1 byte
  statusGP = Wire.read();

  return (statusGP & (0x0001 << pino)) == 0 ? 0 : 1;
}

//-------------------------------TESTA SAIDAS PLACA B----------------------------------
//
void testa_saidas_placa_B()
{
  uint8_t leitura_pino = 0;
  char buffer[100];

  for (int i = 0; i < 8; i++)
  {
    i2c_escreve_pino_unico(EXPANDER_SAIDA_PLACA_B, EXPANDER_GP0, i, 1);
    delay(250);
    leitura_pino = i2c_ler_pino_unico(EXPANDER_ENTRADA_GIGA, EXPANDER_GP0, i);
    if (leitura_pino == 1)
    {
      sprintf(buffer, "Saida acionanda %d ", i + 1);
      Serial.println(buffer);
    }
    else if (leitura_pino == 0)
    {
      sprintf(buffer, "Saida nao acionanda %d ", i + 1);
      Serial.println(buffer);
    }

    delay(1550);
    i2c_escreve_pino_unico(EXPANDER_SAIDA_PLACA_B, EXPANDER_GP0, i, 0);
  }

  for (int i = 0; i < 4; i++)
  {
    i2c_escreve_pino_unico(EXPANDER_SAIDA_PLACA_B, EXPANDER_GP1, i, 1);
    delay(250);
    leitura_pino = i2c_ler_pino_unico(EXPANDER_ENTRADA_GIGA, EXPANDER_GP1, i);
    if (leitura_pino == 1)
    {
      sprintf(buffer, "Saida acionanda %d ", i + 9);
      Serial.println(buffer);
    }
    else if (leitura_pino == 0)
    {
      sprintf(buffer, "Saida nao acionanda %d ", i + 9);
      Serial.println(buffer);
    }
    delay(1550);
    i2c_escreve_pino_unico(EXPANDER_SAIDA_PLACA_B, EXPANDER_GP1, i, 0);
  }
}

//--------------------------------TESTA ENTRADAS DA PLACA B------------------------------
void testa_entradas_placa_B()
{
}
