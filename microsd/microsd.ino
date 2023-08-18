#include <SPI.h>
#include <SD.h>

//Pino de conexão do pino CS do modulo
const int chipSelect = 53;

int nameFileNumb = 1;
char nameDataFile[6];

void setup(){
  Serial.begin(9600);
  Serial.println("Inicializando cartao SD...");
  //Inicia a comunicacao com o modulo SD
  if (!SD.begin(chipSelect))
  {
    Serial.println("Falha ao acessar o cartao !");
    Serial.println("Verifique o cartao/conexoes e reinicie o Arduino...");
    return;
  }
  Serial.println("Cartao iniciado corretamente !");
  Serial.println();
  const char *extension = ".csv";
  do{
    itoa(nameFileNumb, nameDataFile, 10);  // (value, Array, base)
    strcat(nameDataFile, extension);  // syntax:  strcat(dest, source)
    nameFileNumb++;
    // if(SD.exists(nameDataFile)){
    //   SD.remove(nameDataFile);
    // }
  }while(SD.exists(nameDataFile));
  Serial.print("Arquivo ");
  Serial.print(nameDataFile);
  Serial.println(" criado.");
}

void loop(){
  unsigned long currentMillis = millis();
  
  //Mostra os dados no Serial Monitor
  Serial.print("Este arduino esta ligado a ");
  Serial.print(currentMillis / 1000);
  Serial.println(" segundos");

  //Abre o arquivo datalog.txt
  File dataFile = SD.open(nameDataFile, FILE_WRITE);

  //Grava as informacoes no arquivo
  if (dataFile)
  {
    dataFile.print("Este arduino esta ligado a ");
    dataFile.print(currentMillis / 1000);
    dataFile.println(" segundos");

    //Fecha o arquivo
    dataFile.close();
  }

  //Exibe um erro se nao conseguir abrir o arquivo
  else
  {
    Serial.print("Erro ao abrir o arquivo ");
    Serial.println(nameDataFile);
  }
  // dataFile = SD.open(nameDataFile);
  // while(dataFile.available())
  //   Serial.write(dataFile.read());
  // dataFile.close();
  delay(2000);
}