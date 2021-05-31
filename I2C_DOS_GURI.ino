//--------------------------------------------------------------------- INCLUDES
#include <Wire.h>

//--------------------------------------------------------------------- DEFINES
#define EXPANDER_1 0x23 //saida placa teste
#define EXPANDER_2 0x27 //entrada placa teste
#define EXPANDER_3 0x20 //saida placa New B
#define EXPANDER_4 0x21 //Entrada placa New B

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
  I2C_CONFIGURA_PORTAS(EXPANDER_1, EXPANDER_IODIR0, OUTPUT);
  I2C_CONFIGURA_PORTAS(EXPANDER_1, EXPANDER_IODIR1, OUTPUT);

  //---------------------------------------- CONFIGURA EXPANDER 2 Entrada placa teste
  I2C_CONFIGURA_PORTAS(EXPANDER_2, EXPANDER_IODIR0, OUTPUT);
  I2C_CONFIGURA_PORTAS(EXPANDER_2, EXPANDER_IODIR1, INPUT);

  //---------------------------------------- CONFIGURA EXPANDER 3 Saida placa new B
  I2C_CONFIGURA_PORTAS(EXPANDER_3, EXPANDER_IODIR0, OUTPUT);
  I2C_CONFIGURA_PORTAS(EXPANDER_3, EXPANDER_IODIR1, OUTPUT);

  //---------------------------------------- CONFIGURA EXPANDER 4 Entrada placa new B
  I2C_CONFIGURA_PORTAS(EXPANDER_4, EXPANDER_IODIR0, INPUT);
  I2C_CONFIGURA_PORTAS(EXPANDER_4, EXPANDER_IODIR1, INPUT);

  //-----------------------------------------
  I2C_ESCREVE_PINOS(EXPANDER_1, EXPANDER_GP0, 0);
  I2C_ESCREVE_PINOS(EXPANDER_1, EXPANDER_GP1, 0);
}

//---------------------------------- LOOP -----------------------------------
void loop()
{
  uint8_t leitura_pino = 0;

  leitura_pino = I2C_LER_PINO_UNICO(EXPANDER_2, EXPANDER_GP1, 7);
  if (leitura_pino == 1)
    I2C_ESCREVE_PINO_UNICO(EXPANDER_2, EXPANDER_GP0, 5, 1);
  else if (leitura_pino == 0)
    I2C_ESCREVE_PINO_UNICO(EXPANDER_2, EXPANDER_GP0, 5, 0);

  I2C_ESCREVE_PINOS(EXPANDER_1, EXPANDER_GP0, 0);
  I2C_ESCREVE_PINOS(EXPANDER_1, EXPANDER_GP1, 0);
  delay(500);
}

//---------------------------------- CONFIGURA EXPANDER  -----------------------------------
void I2C_CONFIGURA_PORTAS(uint8_t endereco, uint8_t comando, uint8_t valor)
{
  if (valor == INPUT)
  {
    I2C_ESCREVE_PINOS(endereco, comando, 0xFF);
  }
  else if (valor == OUTPUT)
  {
    I2C_ESCREVE_PINOS(endereco, comando, 0x00);
  }
  else
  {
    I2C_ESCREVE_PINOS(endereco, comando, valor);
  }
}

//---------------------------------- ESCREVE PINO UNICO -----------------------------------
// I2C_ESCREVE_PINO_UNICO (endereço_da_expander, comando_expander, pino_escrita, valor)
void I2C_ESCREVE_PINO_UNICO(uint8_t endereco, uint8_t comando, int pino, int valor)
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
// I2C_ESCREVE_PINOS (endereço_da_expander, comando_expander, valor_portas)
void I2C_ESCREVE_PINOS(uint8_t endereco, uint8_t comando, uint8_t dados)
{
  Wire.beginTransmission(endereco);
  Wire.write(comando);
  Wire.write(dados);
  Wire.endTransmission();
  delay(10);
}

//---------------------------------- LEITURA PINO UNICO -----------------------------------
// I2C_LER_PINO_UNICO (endereço_da_expander, comando_expander, valor_portas)
uint8_t I2C_LER_PINO_UNICO(uint8_t endereco, uint8_t porta, uint8_t pino)
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
void TESTA_SAIDAS_PLACA_B()
{
  uint8_t leitura_pino = 0;

  for (int i = 0; i < 8; i++)
  {
    I2C_ESCREVE_PINO_UNICO(EXPANDER_3, EXPANDER_GP0, i, 1);
    delay(250);
    leitura_pino = I2C_LER_PINO_UNICO(EXPANDER_2, EXPANDER_GP0, i);
    if (leitura_pino == 1)
      Serial.println ("\n Saida %d acionanda.", i);
    else if (leitura_pino == 0)
      Serial.println ("\n Saida %d nao acionando.", i);
  }

  for (int i = 0; i < 4; i++)
  {
    I2C_ESCREVE_PINO_UNICO(EXPANDER_3, EXPANDER_GP1, i, 1);
    delay(250);
    leitura_pino = I2C_LER_PINO_UNICO(EXPANDER_2, EXPANDER_GP1, i);
    if (leitura_pino == 1)
      Serial.println ("\n Saida %d acionanda.", i + 8);
    else if (leitura_pino == 0)
      Serial.println ("\n Saida %d nao acionando.", i +8);
  }
}

//--------------------------------TESTA ENTRADAS DA PLACA B------------------------------
void TESTA_ENTRADAS_PLACA_B ()
{


}
