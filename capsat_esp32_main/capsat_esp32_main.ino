/* === Bibliotecas === */

/* === UART === */
#include <HardwareSerial.h>

// Sensor de particulados PMSA003
#include <Adafruit_PM25AQI.h>

// GPS NEO-M8N
#include <TinyGPS++.h>


/* === SPI === */
// Leitor/gravador Micro SD
#include <SdFat.h>


/* === ThreeWire === */
#include <ThreeWire.h>

// Relógio MH-RTC 2
#include <RtcDS1302.h>


/* === I²C === */
// Acelerômetro/giroscópio/magnetômetro MPU-9250
#include <mpu6500.h>

// Altímetro/barômetro MS5611
#include <MS5611.h>

// Sensor de CO2 e umidade ENS160+AHT21
#include <DFRobot_ENS160.h>
#include <AHTxx.h>

// Voltímetro INA219
#include <INA219_WE.h>


/* === Analógicos === */
// Gasômetro MiCS-6814
#include <MICS6814.h>

// Ozonoscópio MQ-131
#include <MQUnifiedsensor.h>

// Termômetro NTC
#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>


/* === Sensores === */
// Acelerômetro/giroscópio MPU-9250
class Acelerometro{
  private:
    bfs::Mpu6500 imu;
  public:
    float dados_acelerometro[6] = {0};
    void iniciar(){
      imu.Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);
      while(!imu.Begin()){// Esperando comunicar com o sensor
        delay(1000);
        Serial.println(F("Aguardando acelerômetro."));
      }
      while(!imu.ConfigSrd(19)){
        delay(1000);
        Serial.println(F("Aguardando giroscópio."));
      }
    }
    void getDados(){
      if(imu.Read()){
        dados_acelerometro[0] = imu.accel_x_mps2();// Acelerômetro eixo X
        dados_acelerometro[1] = imu.accel_y_mps2();// Acelerômetro eixo Y
        dados_acelerometro[2] = imu.accel_z_mps2();// Acelerômetro eixo Z
        dados_acelerometro[3] = imu.gyro_x_radps();// Giroscópio eixo X
        dados_acelerometro[4] = imu.gyro_y_radps();// Giroscópio eixo Y
        dados_acelerometro[5] = imu.gyro_z_radps();// Giroscópio eixo Z
      }
    }
    void printDados(){
      Serial.print(F("Acelerômetro/giroscópio: "));
      for(uint8_t i=0; i<6; i++){
        Serial.print(dados_acelerometro[i]);
        Serial.print(F(" "));
      }
    }
};

// Altímetro/barômetro MS5611
class Altimetro{
  private:
    MS5611 baro;
  public:
    float dados_altimetro[2] = {0};
    void iniciar(){
      baro = MS5611();
      baro.begin();
      baro.pressureOffset = -450;// CALIBRAR de acordo com a pressão local em Pascal (Pa)
    }
    void getDados(){
      dados_altimetro[0] = baro.getPressure();// Pressão em Pascal (Pa)
      dados_altimetro[1] = baro.getAltitude();// Altitude em metros (m)
    }
    void printDados(){
      Serial.print(F("Altímetro/barômetro: "));
      for(uint8_t i=0; i<3; i++){
        Serial.print(dados_altimetro[i]);
        Serial.print(F(" "));
      }
      Serial.println();
    }
};

// Gasômetro MiCS-6814
MICS6814 gas(A7, A3, A6);

class Gasometro{
  public:
    float dados_gasometro[8] = {0};
    void iniciar(){
      gas.calibrate();
    }
    void getDados(){
      dados_gasometro[0] = gas.measure(NH3);
      dados_gasometro[1] = gas.measure(CO);
      dados_gasometro[2] = gas.measure(NO2);
      dados_gasometro[3] = gas.measure(C3H8);
      dados_gasometro[4] = gas.measure(C4H10);
      dados_gasometro[5] = gas.measure(CH4);
      dados_gasometro[6] = gas.measure(H2);
      dados_gasometro[7] = gas.measure(C2H5OH);
    }
    void printDados(){
      Serial.print(F("Gasômetro: "));
      for(uint8_t i=0; i<8; i++){
        Serial.print(dados_gasometro[i],4);
        Serial.print(F(" "));
      }
      Serial.println();
    }
};

// GPS NEO-M8N
HardwareSerial gpsSerial(9, 8);
TinyGPSPlus gps;

class Gps{
  private:
    void getInfo(){
      if (gps.location.isValid()){
        dados_gps[0] = gps.location.lat();// Latitude em graus (°)
        dados_gps[1] = gps.location.lng();// Longitude em graus (°)
      }
      if(gps.altitude.isValid())
        dados_gps[2] = gps.altitude.meters();// Altitude no solo em metros (m)
      if(gps.speed.isValid())
        dados_gps[3] = gps.speed.kmph();// Velocidade em quilômetros por hora (km/h)
      if(gps.course.isValid())
        dados_gps[4] = gps.course.deg();// Direção em graus (°)
    }
  public:
    double dados_gps[5] = {0};
    void iniciar(){
      gpsSerial.begin(9600);
    }
    void getDados(){
      uint16_t i = 0;
      while (gpsSerial.available() > 0){
        if (gps.encode(gpsSerial.read()))
          getInfo();
        if (i > 5000 && gps.charsProcessed() < 10){
          Serial.println("Sinal GPS não detectado.");
          break;
        }
        i++;
      }
    }
    void printDados(){
      Serial.print(F("GPS: "));
      uint8_t i;
      for(i=0; i<2; i++){
        Serial.print(dados_gps[i], 6);
        Serial.print(F(" "));
      }
      for(i=2; i<5; i++){
        Serial.print(dados_gps[i]);
        Serial.print(F(" "));
      }
      Serial.println();
    }
};

// Leitor/gravador Micro SD
class Gravador{
  private:
    SdFat sd;
    SdFile myFile;
    char nameDataFile[25];
  public:
    void iniciar(char* _nameDataFile){
      strcpy(nameDataFile, _nameDataFile);
      strcat(nameDataFile, ".csv");
      while(!sd.begin(10, SPI_FULL_SPEED)){
        Serial.println(F("Aguardando MicroSD."));
        delay(1000);
      }
    }
    SdFile* getDados(){
      if(myFile.open(nameDataFile, O_RDWR | O_CREAT | O_AT_END))
        return &myFile;
      else{
        Serial.println(F("Falha no cartão MicroSD."));
        return nullptr;
      }
    }
};

// Ozonoscópio MQ-131
MQUnifiedsensor MQ131("Arduino NANO", 5, 10, A0, "MQ-131");

class Ozonoscopio{
  public:
    void iniciar(){
      MQ131.setRegressionMethod(1); //_PPM =  a*ratio^b
      MQ131.setA(23.943);
      MQ131.setB(-1.11); // Configure the equation to to calculate O3 concentration
      MQ131.init();
      float calcR0 = 0;
      for(int i = 1; i<=10; i++)
      {
        MQ131.update(); // Update data, the arduino will read the voltage from the analog pin
        calcR0 += MQ131.calibrate(15);
        Serial.print(F("."));
      }
      MQ131.setR0(calcR0/10);
      while(isinf(calcR0)){
        delay(1000);
        Serial.println(F("Aguardando ozonoscópio."));
      }
      while(calcR0 == 0){
        delay(1000);
        Serial.println(F("Aguardando ozonoscópio."));
      }
      MQ131.serialDebug(true);
      MQ131.setOffset(0.1);
    }
    void getDados(){
      MQ131.update();
      MQ131.readSensorR0Rs();
      Serial.print(MQ131.getConcentration(),4);
      Serial.println(F(" ppb"));
    }
};

// Sensor de particulados PMSA003
HardwareSerial pmSerial(2, 3);

class Particulados{
  private:
    Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
  public:
    uint16_t dados_particulados[12] = {0};
    void iniciar(){
      pmSerial.begin(9600);
      while(!aqi.begin_UART(&pmSerial)){
        delay(1000);
        Serial.println("Aguardando o sensor de particulados.");
      }
    }
    void getDados(){
      PM25_AQI_Data data;
      if(aqi.read(&data)){
        /* Concentrações à pressão do nível do mar */
        dados_particulados[0] = data.pm10_standard;// PM1 (<1 micrômetro de diâmetro)
        dados_particulados[1] = data.pm25_standard;// PM2.5 (<2,5 micrômetros de diâmetro)
        dados_particulados[2] = data.pm100_standard;// PM10 (<10 micrômetros de diâmetro)
        /* Concentrações à pressão do ambiente */
        dados_particulados[3] = data.pm10_env;// PM1 (<1 micrômetro de diâmetro)
        dados_particulados[4] = data.pm25_env;// PM2.5 (<2,5 micrômetros de diâmetro)
        dados_particulados[5] = data.pm100_env;// PM10 (<10 micrômetros de diâmetro)
        /* Partículas */
        dados_particulados[6] = data.particles_03um;// > 0.3um / 0.1L ar
        dados_particulados[7] = data.particles_05um;// > 0.5um / 0.1L ar
        dados_particulados[8] = data.particles_10um;// > 1.0um / 0.1L ar
        dados_particulados[9] = data.particles_25um;// > 2.5um / 0.1L ar
        dados_particulados[10] = data.particles_50um;// > 5.0um / 0.1L ar
        dados_particulados[11] = data.particles_100um;// > 10 um / 0.1L ar
      }
    }
    void printDados(){
      Serial.print(F("Sensor de particulados: "));
      for(uint8_t i=0; i<12; i++){
        Serial.print(dados_particulados[i]);
        Serial.print(F(" "));
      }
    }
};

// Radiômetro UV Taidacent
class Radiometro{
  private:
    uint8_t nivelUV;
  public:
    void getDados(){
      double tensaoConvertida = 1000.0*analogRead(A7)*5.0/1023.0;
      if(tensaoConvertida <= 227)
        nivelUV = 0;
      else if(tensaoConvertida > 227 && tensaoConvertida <= 318)
        nivelUV = 1;
      else if(tensaoConvertida > 318 && tensaoConvertida <= 408)
        nivelUV = 2;
      else if(tensaoConvertida > 408 && tensaoConvertida <= 503)
        nivelUV = 3;
      else if(tensaoConvertida > 503 && tensaoConvertida <= 606)
        nivelUV = 4;
      else if(tensaoConvertida > 606 && tensaoConvertida <= 696)
        nivelUV = 5;
      else if(tensaoConvertida > 696 && tensaoConvertida <= 795)
        nivelUV = 6;
      else if(tensaoConvertida > 795 && tensaoConvertida <= 881)
        nivelUV = 7;
      else if(tensaoConvertida > 881 && tensaoConvertida <= 976)
        nivelUV = 8;
      else if(tensaoConvertida > 976 && tensaoConvertida <= 1079)
        nivelUV = 9;
      else if(tensaoConvertida > 1079 && tensaoConvertida <= 1170)
        nivelUV = 10;
      else
        nivelUV = 11;
      Serial.print(F("UV "));
      Serial.println(nivelUV);
      Serial.println();
    }
};

// Relógio MH-RTC 2
ThreeWire myWire(6, 7, 5); // OBJETO DO TIPO ThreeWire
RtcDS1302<ThreeWire> Rtc(myWire); // OBJETO DO TIPO RtcDS1302

class Relogio{
  public:
    char dados_relogio[21] = {0};
    void iniciar(){
      if(Rtc.GetIsWriteProtected())// SE O RTC ESTIVER PROTEGIDO CONTRA GRAVAÇÃO, FAZ
        Rtc.SetIsWriteProtected(false);// HABILITA GRAVAÇÃO NO RTC
      if(!Rtc.GetIsRunning())// SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
        Rtc.SetIsRunning(true);// INICIALIZA O RTC
      Rtc.SetDateTime(RtcDateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour()-3, gps.time.minute(), gps.time.second()));// VARIÁVEL RECEBE DATA E HORA DE COMPILAÇÃO
      Rtc.SetIsWriteProtected(true);// DESABILITA GRAVAÇÃO NO RTC
    }
    void getDados(){
      RtcDateTime datetime = Rtc.GetDateTime();
      snprintf_P(dados_relogio,
                 21,
                 PSTR("%02u-%02u-%04u_%02u-%02u-%02u"),// FORMATO DE EXIBIÇÃO DAS INFORMAÇÕES
                 datetime.Day(),// DIA
                 datetime.Month(),// MÊS
                 datetime.Year(),// ANO
                 datetime.Hour(),// HORA
                 datetime.Minute(),// MINUTOS
                 datetime.Second());// SEGUNDOS
    }
    void printDados(){
      Serial.print(F("Relógio: "));
      Serial.print(dados_relogio);// IMPRIME NO MONITOR SERIAL AS INFORMAÇÕES
      Serial.println();// QUEBRA DE LINHA NA SERIAL
    }
};

// Termômetro NTC
class Termometro{
  private:
    Thermistor* originThermistor = nullptr;
    Thermistor* thermistor = nullptr;
    double celsius;
  public:
    void iniciar(){
      originThermistor = new NTC_Thermistor(A2, 9870, 10000, 25, 3950);
      thermistor = new SmoothThermistor(originThermistor, 5);
    }
    void getDados(){
      celsius = thermistor->readCelsius();
      Serial.print(F("Temperature: "));
      Serial.print(celsius);
      Serial.println(F(" C, "));
      Serial.println();
    }
};

// Sensor de CO2 e umidade ENS160+AHT21
AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);

class Umidade{
  private:
    float ahtTemperature;
    float ahtHumidity;
    void printStatus(){
      switch(aht20.getStatus()){
        case AHTXX_NO_ERROR:
          Serial.println(F("no error"));
          break;
        case AHTXX_BUSY_ERROR:
          Serial.println(F("sensor busy, increase polling time"));
          break;
        case AHTXX_ACK_ERROR:
          Serial.println(F("sensor didn't return ACK, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
          break;
        case AHTXX_DATA_ERROR:
          Serial.println(F("received data smaller than expected, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
          break;
        case AHTXX_CRC8_ERROR:
          Serial.println(F("computed CRC8 not match received CRC8, this feature supported only by AHT2x sensors"));
          break;
        default:
          Serial.println(F("unknown status"));
          break;
      }
    }
  public:
    void iniciar(){
      while(aht20.begin() != true){
        delay(1000);
        Serial.println(F("Aguardando sensor de umidade."));
      }
      Serial.println(F("AHT20 OK"));
      Serial.println();
      Serial.println(F("DEMO 1: read 12-bytes"));
      ahtTemperature = aht20.readTemperature();
      Serial.print(F("Temperature: "));
      if(ahtTemperature != AHTXX_ERROR){
        Serial.print(ahtTemperature);
        Serial.println(F(" +-0.3C"));
      }
      else{
        printStatus();
        if(aht20.softReset() == true)
          Serial.println(F("reset success"));
        else
          Serial.println(F("reset failed"));
      }
      delay(2000);
      ahtHumidity = aht20.readHumidity();
      Serial.print(F("Humidity: "));
      if (ahtHumidity != AHTXX_ERROR){
        Serial.print(ahtHumidity);
        Serial.println(F(" +-2%"));
      }
      else{
        printStatus();
      }
      delay(200);
      // Serial.println();
      // Serial.println(F("DEMO 2: read 6-byte"));
      // ahtValue = aht20.readTemperature();
      // Serial.print(F("Temperature: "));
      // if(ahtValue != AHTXX_ERROR){
      //   Serial.print(ahtValue);
      //   Serial.println(F(" +-0.3C"));
      // }
      // else{
      //   printStatus();
      // }
      // ahtValue = aht20.readHumidity(AHTXX_USE_READ_DATA);
      // Serial.print(F("Humidity: "));
      // if (ahtValue != AHTXX_ERROR){
      //   Serial.print(ahtValue);
      //   Serial.println(F(" +-2%"));
      // }
      // else{
      //   printStatus();
      // }
      // delay(10000);
      while(NO_ERR != ENS160.begin()){
        delay(1000);
        Serial.println(F("Aguardando o sensor de CO2."));
      }
      Serial.println(F("Begin ok!"));
      ENS160.setPWRMode(ENS160_STANDARD_MODE);
      ENS160.setTempAndHum(ahtTemperature, ahtHumidity);
      uint8_t Status = ENS160.getENS160Status();
      Serial.print(F("Sensor operating status : "));
      Serial.println(Status);
    }
    void getDados(){
      uint8_t AQI = ENS160.getAQI();
      Serial.print(F("Air quality index : "));
      Serial.println(AQI);
      uint16_t TVOC = ENS160.getTVOC();
      Serial.print(F("Concentration of total volatile organic compounds : "));
      Serial.print(TVOC);
      Serial.println(F(" ppb"));
      uint16_t ECO2 = ENS160.getECO2();
      Serial.print(F("Carbon dioxide equivalent concentration : "));
      Serial.print(ECO2);
      Serial.println(F(" ppm"));
      Serial.println();
    }
};

// Voltímetro INA219
class Voltimetro{
  private:
    INA219_WE tensao = INA219_WE(0x40);
  public:
    float dados_voltimetro[2] = {0};
    void iniciar(){
      while(!tensao.init()){
        delay(1000);
        Serial.println(F("Aguardando o voltímetro."));
      }
      tensao.setADCMode(SAMPLE_MODE_128); // choose mode and uncomment for change of default
      tensao.setMeasureMode(CONTINUOUS); // choose mode and uncomment for change of default
      tensao.setPGain(PG_40); // choose gain and uncomment for change of default
      tensao.setShuntSizeInOhms(0.01);
    }
    void getDados(){
      dados_voltimetro[0] = (float)map(tensao.readRegister(0x02),2,610,234,1253)/100.0;// Tensão elétrica em volts
      dados_voltimetro[1] = constrain((float)map((long)(dados_voltimetro[0]*100.0), 1000, 1680, 0, 10000)/100.0, 0.0, 100.0);// Porcentagem da bateria
    }
    void printDados(){
      for(uint8_t i=0; i<2; i++){
        Serial.print(dados_voltimetro[i]);
        Serial.print(F(" "));
      }
      Serial.println();
    }
};

Acelerometro mpu9250;
Altimetro ms5611;
Gasometro mics6814;
Gps m8n;
Gravador microsd;
Ozonoscopio mq131;
Particulados pmsa3;
Radiometro taidacent;
Relogio mhrtc2;
Termometro ntc;
Umidade ens160aht21;
Voltimetro ina219;

void inicializarComponentes(){
  Wire.begin();// Iniciar I²C
  Wire.setClock(400000);
  m8n.iniciar();
  do{
    m8n.getDados();
    delay(1000);
    Serial.println(F("Buscando sinal GPS."));
  }while(!gps.date.isValid() && !gps.time.isValid());
  mhrtc2.iniciar();
  pmsa3.iniciar();
  ms5611.iniciar();
  mpu9250.iniciar();
  mics6814.iniciar();
  ntc.iniciar();
  ina219.iniciar();
  mq131.iniciar();
  ens160aht21.iniciar();
  mhrtc2.getDados();
  microsd.iniciar(mhrtc2.dados_relogio);
}

void getTodos(){
  m8n.getDados();
  mhrtc2.getDados();
  pmsa3.getDados();
  ms5611.getDados();
  mpu9250.getDados();
  ntc.getDados();
  ina219.getDados();
  mq131.getDados();
  mics6814.getDados();
  taidacent.getDados();
  SdFile* _dataFile = microsd.getDados();
  if(_dataFile){
    uint8_t i;
    _dataFile->print(mhrtc2.dados_relogio);
    _dataFile->print(F(","));
    for(i=0; i<2; i++){
      _dataFile->print(ina219.dados_voltimetro[i]);
      _dataFile->print(F(","));
    }
    for(i=0; i<3; i++){
      _dataFile->print(m8n.dados_gps[i],6);
      _dataFile->print(F(","));
    }
    for(i=3; i<5; i++){
      _dataFile->print(m8n.dados_gps[i]);
      _dataFile->print(F(","));
    }
    for(i=0; i<6; i++){
      _dataFile->print(mpu9250.dados_acelerometro[i]);
      _dataFile->print(F(","));
    }
    _dataFile->print(qmc5883l.dados_magnetometro);
    _dataFile->print(F(","));
    for(i=0; i<2; i++){
      _dataFile->print(ms5611.dados_altimetro[i]);
      _dataFile->print(F(","));
    }
    _dataFile->print(ntc.dados_termometro);
    _dataFile->print(F(","));
    for(i=0; i<8; i++){
      _dataFile->print(mics6814.dados_gasometro[i],4);
      _dataFile->print(F(","));
    }
    _dataFile->print(mq131.dados_ozonoscopio,4);
    _dataFile->print(F(","));
    for(i=0; i<3; i++){
      _dataFile->print(ens160aht21.dados_sensorco2[i],4);
      _dataFile->print(F(","));
    }
    for(i=0; i<12; i++){
      _dataFile->print(pmsa3.dados_particulados[i]);
      _dataFile->print(F(","));
    }
    _dataFile->println();
    _dataFile->close();
  }
}

void printTodos(){
  m8n.printDados();
  mhrtc2.printDados();
  pmsa3.printDados();
  ms5611.printDados();
  mpu9250.printDados();
  ntc.printDados();
  ina219.printDados();
  mq131.printDados();
  mics6814.printDados();
  taidacent.printDados();
  Serial.println();
}

void setup(){
  Serial.begin(9600);
  inicializarComponentes();
  delay(2);
}

void loop(){
  getTodos();
  printTodos();
  delay(1000);
}