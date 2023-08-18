#define PIN_NH3 A0// Vh=2.2V
#define PIN_NO2 A1// =PIN_OX, Vh=1.7V
#define PIN_CO A2// =PIN_RED, Vh=2.2V

typedef enum{
	CO,
	NO2,
	NH3,
	C3H8,
	C4H10,
	CH4,
	H2,
	C2H5OH,
  NO,
  H2S
}tipo;

long leituraCO;
long leituraNO2;
long leituraNH3;
long RsR0RED;
long RsR0OX;
long RsR0NH3;

double medir(tipo gas){
  switch(gas){
    case CO:
      return constrain((double)map(RsR0RED, 35000, 100, 10000, 10000000)/10000.0, 1.0, 1000.0);
      break;
    case NO2:
      return constrain((double)map(RsR0OX, 620, 420000, 100, 65000)/10000.0, 0.01, 6.5);
      break;
    case NH3:
      return constrain((double)map(RsR0NH3, 7500, 550, 10000, 1600000)/10000.0, 1.0, 160.0);
      break;
    case C3H8:
      return constrain((double)map(RsR0NH3, 7800, 2500, 10000000, 250000000)/10000.0, 1000.0, 25000.0);
      break;
    case C4H10:
      return constrain((double)map(RsR0NH3, 6800, 1600, 10000000, 250000000)/10000.0, 1000.0, 25000.0);
      break;
    case CH4:
      return constrain((double)map(RsR0RED, 7200, 5000, 30000000, 150000000)/10000.0, 3000.0, 15000.0);
      break;
    case H2:
      double aux = constrain((double)map(RsR0RED, 8000, 400, 12000, 2600000)/10000.0, 1.2, 260);
      if(aux == 1.2)
        return constrain((double)map(RsR0NH3, 20000, 3400, 10000, 2000000)/10000.0, 1.0, 200.0);
      else
        return aux;
      break;
    case C2H5OH:
      return constrain((double)map(RsR0NH3, 6000, 750, 10000, 2600000)/10000.0, 1.0, 260.0);
      break;
    case NO:
      return constrain((double)map(RsR0OX, 10000, 7000, 1000, 15000)/10000.0, 0.1, 1.5);
      break;
    case H2S:
      return constrain((double)map(RsR0RED, 7000, 6000, 1000, 5000)/10000.0, 0.1, 0.5);
      break;
    default:
      return -1.0;
      break;
  }
}

void setup(){
  Serial.begin(9600);
}

void loop(){
  leituraCO = analogRead(PIN_CO);//Vmax=2,4V
  leituraNO2 = analogRead(PIN_NO2);//Vmax=1,7V
  leituraNH3 = analogRead(PIN_NH3);//Vmax=2,2V
  RsR0RED = map(leituraCO, 0, 491, 100, 35000);//x10000
  RsR0OX = map(leituraNO2, 0, 348, 6200, 420000);//x10000
  RsR0NH3 = map(leituraNH3, 0, 455, 4200, 200000);//x10000
  Serial.print("NH3: ");
  Serial.print(medir(NH3));
  Serial.print("ppm\t");
  Serial.print("CO: ");
  Serial.print(medir(CO));
  Serial.print(" ppm\t");
  Serial.print("NO2: ");
  Serial.print(medir(NO2));
  Serial.print(" ppm\t");
  Serial.print("C3H8: ");
  Serial.print(medir(C3H8));
  Serial.print(" ppm\t");
  Serial.print("C4H10: ");
  Serial.print(medir(C4H10));
  Serial.print(" ppm\t");
  Serial.print("CH4: ");
  Serial.print(medir(CH4));
  Serial.print(" ppm\t");
  Serial.print("H2: ");
  Serial.print(medir(H2));
  Serial.print(" ppm\t");
  Serial.print("C2H5OH: ");
  Serial.print(medir(C2H5OH));
  Serial.println(" ppm\t");
  delay(1000);
}
