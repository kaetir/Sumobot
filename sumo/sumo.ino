/*

	 #    #    #                      #           #       ###   ####   #####
	 #    #    #                     # #          #        #    #   #  #
	#     #     #                   #   #         #        #    #   #  #
   #      #      #                  #   #         #        #    ####   ####
  #       #       #                 #####         #        #    # #    #
 #                 #                #   #         #        #    #  #   #
 #        #        #                #   #         #####   ###   #   #  #####


Pour les moteurs :
	0 correspond au gauche
	1 correspond au droit



Pour les Stratégies :
	1 0 0 enemie devant a gauche
	0 1 0 enemie devant a devant
	0 0 1 enemie devant a droite
	0 0 0 enemie derière #pasDansLesFesses




uncomment for debug

*/

//#define debug 1

#include <Servo.h>


// 8888b.  888888  dP""b8 88        db    88""Yb    db    888888 88  dP"Yb  88b 88     8888b.  888888 .dP"Y8     88""Yb 88 88b 88 .dP"Y8
//  8I  Yb 88__   dP   `" 88       dPYb   88__dP   dPYb     88   88 dP   Yb 88Yb88      8I  Yb 88__   `Ybo."     88__dP 88 88Yb88 `Ybo."
//  8I  dY 88""   Yb      88  .o  dP__Yb  88"Yb   dP__Yb    88   88 Yb   dP 88 Y88      8I  dY 88""   o.`Y8b     88"""  88 88 Y88 o.`Y8b
// 8888Y"  888888  YboodP 88ood8 dP""""Yb 88  Yb dP""""Yb   88   88  YbodP  88  Y8     8888Y"  888888 8bodP'     88     88 88  Y8 8bodP'

// moteur gauche

#define moteurGauche        0
#define moteurGaucheEn      6   // pwm controle vitesse
#define moteurGaucheAvant   7
#define moteurGaucheArriere 8


// moteur droit
#define moteurDroit 		1
#define moteurDroitEn   	5    // pwm contole vitesse
#define moteurDroitAvant	3
#define moteurDroitArriere	4

// capteur de ligne blanche    0 blanc    1 noir
#define capteurLigneDroit 	A2
#define capteurLigneGauche  A1


// capteur adversaire
#define capteurDroit 	A7
#define capteurAvant 	A6
#define capteurGauche 	A5


// DIP SWITCH
#define DIP1 10
#define DIP2 11
#define DIP3 12


// start button
#define start 2



/*
8888b.  888888  dP""b8 88        db    88""Yb    db    888888 88  dP"Yb  88b 88     8888b.  888888 .dP"Y8     888888 Yb  dP 88""Yb 888888 .dP"Y8
 8I  Yb 88__   dP   `" 88       dPYb   88__dP   dPYb     88   88 dP   Yb 88Yb88      8I  Yb 88__   `Ybo."       88    YbdP  88__dP 88__   `Ybo."
 8I  dY 88""   Yb      88  .o  dP__Yb  88"Yb   dP__Yb    88   88 Yb   dP 88 Y88      8I  dY 88""   o.`Y8b       88     8P   88"""  88""   o.`Y8b
8888Y"  888888  YboodP 88ood8 dP""""Yb 88  Yb dP""""Yb   88   88  YbodP  88  Y8     8888Y"  888888 8bodP'       88    dP    88     888888 8bodP'

888888 888888     888888  dP"Yb  88b 88  dP""b8 888888 88  dP"Yb  88b 88 .dP"Y8
88__     88       88__   dP   Yb 88Yb88 dP   `"   88   88 dP   Yb 88Yb88 `Ybo."
88""     88       88""   Yb   dP 88 Y88 Yb        88   88 Yb   dP 88 Y88 o.`Y8b
888888   88       88      YbodP  88  Y8  YboodP   88   88  YbodP  88  Y8 8bodP'
*/


typedef enum moteur
{
	gauche,
	droit
}moteur ;


typedef enum sens
{
	arriere,
	avant
}sens ;




Servo moteurGaucheVitesse;
Servo moteurDroitVitesse;


/*
** active un moteur dans un sens et une vitesse
** m : moteur
** sens le sens : false arriere, true avant
** vitesse : une valeur de pwm entre 0 et 255
**/
void setMoteur(moteur m, sens s, size_t vitesse){
	if (m == moteurDroit){
		digitalWrite(moteurDroitAvant, s);
		digitalWrite(moteurDroitArriere, !s);
		moteurDroitVitesse.write(vitesse);
	}
	else if (m == moteurGauche){
		digitalWrite(moteurGaucheAvant, s);
		digitalWrite(moteurGaucheArriere, !s);
		moteurGaucheVitesse.write(vitesse);
	}
}

void stopMoteur(moteur m){
	if (m == moteurDroit){
		digitalWrite(moteurDroitAvant, false);
		digitalWrite(moteurDroitArriere, false);
		moteurDroitVitesse.write(0);
	}
	else if (m == moteurGauche){
		digitalWrite(moteurGaucheAvant, false);
		digitalWrite(moteurGaucheArriere, false);
		moteurGaucheVitesse.write(0);
	}

}


/*
** 0 si pas de ligne
** 1 si droite
** 2 si gauche
**/
int getLigne(){

	#ifdef debug
	Serial.println(String("G : ") + analogRead(capteurLigneGauche) + "\t D : " + analogRead(capteurLigneDroit));
	#endif

	if(analogRead(capteurLigneGauche) < 800)  return 2;
	if(analogRead(capteurLigneDroit)  < 700)  return 1;
	return  0;
}


/*
** codes :
** 		- 0 pas vu (surement derière)
** 		- 1 droite
** 		- 2 devant
** 		- 3 gauche
**/
size_t getAdversaire(){

	 #ifdef debug
	Serial.println(String("adv G : ") + analogRead(capteurGauche) + "\t adv D : " + analogRead(capteurDroit)+ "\t adv A : " + analogRead(capteurAvant));
	 #endif

	if(analogRead(capteurGauche) > 600 ) return 3;
	if(analogRead(capteurDroit)  < 600 ) return 1;
	if(analogRead(capteurAvant)  < 600 ) return 2;
	return 0;

}



size_t dipToInt(){
	size_t val= 0;


	val += !digitalRead(DIP1);
	val <<= 1;
	val += !digitalRead(DIP2);
	val <<= 1;
	val += !digitalRead(DIP3);


	return val;

}


/*
** une fois que le jeu a commencé on entre dans la routine qui a pour but de dégager le pd
**/
void routine(){
	while(true){
		if(getAdversaire()== 2){  // FONCER
			setMoteur(droit  , avant  , 255 );
			setMoteur(gauche , avant  , 255 );
			while (getAdversaire()== 2);
		}
		if(getAdversaire()==0 ){ // #pasDansLesFesses
			setMoteur(droit  , avant , 255 );
			setMoteur(gauche , avant , 255 );
		}
		if(getAdversaire()== 1){  // À DROITE
			setMoteur(droit  , arriere, 255 );
			setMoteur(gauche , avant  , 255 );
			while (getAdversaire()!= 2) ;
			setMoteur(droit  , avant  , 255 );
			setMoteur(gauche , arriere, 255 );
			delay(70);			
		}
		if(getAdversaire()== 3){  // À GAUCHE
			setMoteur(droit  , avant  , 255 );
			setMoteur(gauche , arriere, 255 );
			while (getAdversaire()!= 2) ;
			setMoteur(droit  , arriere, 255 );
			setMoteur(gauche , avant  , 255 );
			delay(70);
		}
		if(getLigne()){
			#ifdef debug
			Serial.println("ligne détecté");
			#endif
			setMoteur(droit  , arriere , 255 );
			setMoteur(gauche , arriere , 255 );
			delay(500);
				if(getLigne() == 1){ // droite detecte : on recule et on repart sur la gauche
					setMoteur(droit  , avant   , 255 );
					setMoteur(gauche , arriere , 255 );
					delay(500);
				}else{
					setMoteur(droit  , arriere , 255 );
					setMoteur(gauche , avant   , 255 );
					delay(500);
				}
			}else{
				setMoteur(droit  , avant  , 255 );
				setMoteur(gauche , avant  , 255 );
			}
		}
	}




/*

 #   #    #    ###    #  #
 ## ##   # #    #     ## #
 # # #  #   #   #     # ##
 # # #  #####   #     #  #
 #   #  #   #   #     #  #
 #   #  #   #  ###    #  #


*/


	void setup(){

	// initilisation des pins moteur en sorties et des pwm
		moteurGaucheVitesse.attach(moteurGaucheEn);
		moteurDroitVitesse.attach(moteurDroitEn);

		pinMode(moteurGaucheAvant, OUTPUT);
		pinMode(moteurGaucheArriere, OUTPUT);

		pinMode(moteurDroitAvant, OUTPUT);
		pinMode(moteurDroitArriere, OUTPUT);

	// definition des dip switch
		pinMode(DIP1, INPUT);
		pinMode(DIP2, INPUT);
		pinMode(DIP3, INPUT);


	// definition des capteurs de ligne blanche
		pinMode(capteurLigneGauche, INPUT_PULLUP);
		pinMode(capteurLigneDroit , INPUT_PULLUP);


	// capteur adversaire
		pinMode(capteurGauche, INPUT);
		pinMode(capteurDroit, INPUT);
		pinMode(capteurAvant, INPUT);


		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, LOW);

		pinMode(start, INPUT);

		Serial.begin(9600);


/*

   #    #####  #####  ####   #  #   #####  ####          ###    ####          ###    ###    ####    ##     ##    ###     ##    #  #          ###    #  #          ####    ##    #  #   #####   ##    #  #           ##    #####    #    ###    #####
  # #     #      #    #      ## #     #    #             #  #   #             #  #   #  #   #      #  #   #  #    #     #  #   ## #          #  #   #  #          #   #  #  #   #  #     #    #  #   ## #          #  #     #     # #   #  #     #
 #   #    #      #    ###    # ##     #    ###           #  #   ###           #  #   #  #   ###     #      #      #     #  #   # ##          #  #   #  #          ####   #  #   #  #     #    #  #   # ##           #       #    #   #  #  #     #
 #####    #      #    #      #  #     #    #             #  #   #             ###    ###    #        #      #     #     #  #   #  #          #  #   #  #          #   #  #  #   #  #     #    #  #   #  #            #      #    #####  ###      #
 #   #    #      #    #      #  #     #    #             #  #   #             #      #  #   #      #  #   #  #    #     #  #   #  #          #  #   #  #          #   #  #  #   #  #     #    #  #   #  #          #  #     #    #   #  #  #     #
 #   #    #      #    ####   #  #     #    ####          ###    ####          #      #  #   ####    ##     ##    ###     ##    #  #          ###     ##           ####    ##     ##      #     ##    #  #           ##      #    #   #  #  #     #

*/

		while(digitalRead(start)){ ; }
		digitalWrite(LED_BUILTIN, HIGH);

		delay(5000);
		

		switch (dipToInt()) {

		case 0: // derière      0 0 0 
		setMoteur(gauche, avant  , 255);
		setMoteur(droit , arriere, 255);
		delay(1000);
		break;

		case 4: // gauche       1 0 0
		setMoteur(gauche, arriere, 255);
		setMoteur(droit , avant  , 255);
		delay(500);
		break;

		case 7:  // devant       1 1 1
		setMoteur(gauche, avant  , 255);
		setMoteur(droit , avant  , 255);
		break;

		case 1:  // à droite     0 0 1
		setMoteur(gauche, avant  , 255);
		setMoteur(droit , arriere, 255);
		delay(500);
		break;
 
		default:
		routine();
	}
	routine();

}



void loop(){

		// oupsi


}
