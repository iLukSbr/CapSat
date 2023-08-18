void setup(){
  Serial.begin(9600);
}

void loop(){
  double tensaoConvertida = 1000.0*analogRead(A7)*5.0/1023.0;
  uint8_t nivelUV;
  //Compara a tensão calculada com valores tabela do nível ultravioleta
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
  Serial.print("UV ");
  Serial.println(nivelUV);
  Serial.println();
}
