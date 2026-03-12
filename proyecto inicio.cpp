/*PROGRAMA QUE SIRVE NO COMO EL DEL INCOMPETENTE DE SAMUEL*/

# include <iostream>
# include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>

using namespace std;

main(){
	
	setlocale(LC_ALL,"spanish");
	//VALORES INICIALES QUE TENDRA EL PROGRAMA	
	int temp_ambiente, p_vapor = 0,p_bomba = 0,nivel=0;
	string bomba = "APAGADA",v1="CERRADA",pt1="ON",tt="ON";
	//VARIABLES A USAR PARA EL TXT	
	string v2,v3,temp,p_vapor_leche;
	//DETERMINAN LOS ULTIMOS 5 ESTAODS DEL NIVEL	
	int bajo_90=0, sobre_90=0;
	//para alertar si estamos contaminando los tanques
	bool contaminacion=false;
	// para mostrar texto de alerta, uno para temperatura y otro para presion de vapor de leche
	bool peligro1=false,peligro2=false;
	
	cout<<"Bienvenido al proceso de pasteurización de leche de PARMALAT"<<endl;
	cout<<"Estamos trabajando en San Joaquín y la temperatura por lo general ronda entre los 20 y 32 grados"<<endl;
	cout<<"Ingrese la temperatura ambiente:";
	cin>>temp_ambiente;
	system("cls");
	
	int temperatura = temp_ambiente;
	
	while(true){
		system("cls");
		ifstream archivo("./texto.txt");
		
		//Verifica si el archivo existe en la ruta especificada, de no funcionar termina el programa
			
		if(!archivo){
			cout<<"No se pudo encontrar el archivo en la ruta especificada"<<endl;
			break;
		}
		
		// variables de txt		
		
		archivo>>v2>>v3>>temp>>p_vapor_leche;

		//	transformacion de string a int, FUNCIONA
		
		int temp_num = atoi(temp.c_str());
		int p_vapor_leche_num = atoi(p_vapor_leche.c_str());
		
		// PASAMOS EL TEXTO DE V2 Y V3 A MAYUSCULA PARA MANEJARLOS APROPIADAMENTE
		
		for(int i = 0; i<v2.length();i++){
			v2[i] = toupper(v2[i]);
		}
		
		for(int i = 0; i<v3.length();i++){
			v3[i] = toupper(v3[i]);
		}
		
		if((v2!="CERRADA" && v2!="ABIERTA") ||(v3!="CERRADA" && v3!="ABIERTA") || (temp_num<0 || temp_num>20)||(p_vapor_leche_num<0 || p_vapor_leche_num>40)){
			system("cls");
			cout<<"\t----------VALORES INVÁLIDOS----------\n"<<endl;
			cout<<"\tEl archivo txt debe presentar los siguientes valores:\n\n";
			cout<<"\t----------Archivo.txt----------\n\n";
			cout<<"ESTADO DE V2       ESTADO DE V3       PERTURBACIÓN DE TEMPERATURA   PERTURBACIÓN DE PRESIÓN DE VAPOR DE LECHE\n";
			cout<<"(ABIERTA/CERRADA)  (ABIERTA/CERRADA)  (0-20)                        (0-40)\n\n";
			cout<<"Presione cualquier tecla para verificar el archivo nuevamente:";
			getch();
		}
		//AQUI OPERA EL PROGRAMA CON NORMALIDAD SI OS VALORES DEL TXT SON VALIDOS		
		else{
			
			//  PROCESO DE VERIFICACION DE VARIABLES	
			
			//	ESTADO DE LA SIMULACION
		
			cout<<"\t----------ESTADO DE LA SIMULACIÓN----------\n"<<endl;
			cout<<"\tLa temperatura en el tanque es: "<<temperatura<<" grados  ";
			if(peligro1){
				cout<<"ALERTA!!!";
			}
			cout<<"\n\tLa presión del vapor de leche es: "<<p_vapor<<" psi  ";
			if(peligro2){
				cout<<"ALERTA!!!"<<endl;
			}
			cout<<"\n\tEl nivel del tanque es: "<<nivel<<"%"<<endl;
			cout<<"\tLa bomba está: "<<bomba<<endl;
			cout<<"\tLa válvula de vapor de agua está: "<<v1<<endl;
			cout<<"\tLa válvula de leche sin pasteurizar está: "<<v2<<endl;
			cout<<"\tLa válvula de salida está: "<<v3<<endl;
			cout<<"\tLa presión a la descarga de la bomba es : "<<p_bomba<<" psi"<<endl;
			cout<<"\tAlarma de temperatura: "<<tt<<endl;
			cout<<"\tAlarma de presión de vapor de leche: "<<pt1<<endl;

		
			//ALERTA DE CONTAMINACION DE TANQUES DE ALMACENAMIENTO		
				
			if(contaminacion){
				cout<<"\n\t----------ALERTA DE CONTAMINACIÓN----------\n"<<endl;
				cout<<"Se está mandando leche sin pasteurizar para los tanques de almacenamiento"<<endl;
				cout<<"Se está procediendo a vaciar el tanque T-01"<<endl;
				system("color 20");
			}
		
			// NIVEL DEL TANQUE T-01
			
			if(v2=="ABIERTA" && bomba=="ENCENDIDA"){
			
				nivel++;
			}
			// calculo de los ultimos 5 estaods del nivel del tanque
			if(nivel<=90){
				if(bajo_90<5){
					bajo_90++;
					sobre_90=0;
				}
			}
			else{
				if(sobre_90<5){
					sobre_90++;
					bajo_90=0;
				}
			}
			
			//APERTURA Y CIERRE DE V1	
				
			if(v2=="ABIERTA" || v3=="ABIERTA" || tt=="OFF" || pt1=="OFF" || sobre_90!=5){
				v1 = "CERRADA";
			}	
			else{
				v1 = "ABIERTA";
			}
			
			// ALERTAS
			
			if(temperatura>100){
	
				tt="OFF";
	
				
			}
			else if(p_vapor>100){
				pt1="OFF";
	
	
			}
			else if(v2=="ABIERTA" && v3=="ABIERTA" && bomba=="ENCENDIDA"){
				bomba="APAGADA";
				contaminacion = true;
	
			}
	
			
			//DESACTIVADOR DE ALARMAS (NO TIENEN QUE VER CON LOS COLORES)
			if(temperatura<=90 && tt=="OFF"){
				tt="ON";
	
			}
			
			if(p_vapor<=90 && pt1=="OFF"){
				pt1="ON";
	
			}
			
			if(contaminacion && nivel<5){
				contaminacion=false;
			}
			
			// COLORES DE LAS ALERTAS PARA QUE DUREN UN POCO MAS DE TIEMPO
			if(temperatura>97 && tt=="OFF"){
				system("color 47");
				peligro1=true;
			}
			else if(p_vapor>96 && pt1=="OFF"){
				system("color e0");	
				peligro2=true;
			}
			else if(contaminacion){
				system("color a0");	
			}
			else{
				system("color 07");
				peligro1=false;
				peligro2=false;
			}
			
			
			
			
			//TEMPERATURA		

			
			if(v1=="ABIERTA" && tt=="ON"){
				if(temperatura!=(95+temp_num)){
					temperatura++;
					
				}
			}
			else{
				if(temperatura>temp_ambiente){
					
					temperatura--;
				}
			}
			
		
			//PRESION DE VAPOR DE LECHE		
			
			
			if(v1=="ABIERTA" && temperatura>80 && pt1=="ON"){
				if(p_vapor>=(95+p_vapor_leche_num)){
					p_vapor=95;
				}
				else{
					
					p_vapor+=4;
				}
				
			}
			else{
				p_vapor-=4;
			}	
			
			if(p_vapor<0){
				p_vapor=0;
			}
			
	
			
			// PRESION DE LA BOMBA 
			
			if(bomba=="ENCENDIDA" && v2 =="ABIERTA"){
				if(p_bomba<40){
					p_bomba++;
				}
				else if(p_bomba>40){
					p_bomba--;
				}
			}
			if(bomba=="APAGADA" && v2 =="ABIERTA"){
				p_bomba--;
			}
			if(bomba=="ENCENDIDA" && v2 =="CERRADA"){
				p_bomba+=2;
			}
			
			if(p_bomba<=0){
				p_bomba=0;
			}
	
			
			// ENCENDIDO Y APAGADO DE LA BOMBA
			
			if(p_bomba<20 && v2=="ABIERTA" && bajo_90==5 && v3=="CERRADA"){
				bomba="ENCENDIDA";
			}
			
			if(p_bomba>60 && bomba=="ENCENDIDA"){
				bomba="APAGADA";
			}
			
			if(sobre_90==5 && bomba=="ENCENDIDA"){
				bomba="APAGADA";
			}
			
			
			// V3 - VALVULA DE LECHE PASTEURIZADA
			
			if(v3=="ABIERTA"){
				nivel-=3;
			}
			
			if(nivel<=0){
				nivel=0;
			}
			
			
			
			Sleep(1000);	
				
			
		}
		
	
	}
	

	system("pause");
}
